#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <linux/videodev2.h>

#include <stdbool.h>
#include <pthread.h>

#include "UDP.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct UDP_socket *sock_udp;

bool quit = false;

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void   *start;
        size_t  length;
};

static char            *dev_name;
static enum io_method   io = IO_METHOD_MMAP;
static int              fd = -1;
struct buffer          *buffers;
static unsigned int     n_buffers;
static int              out_buf;
static int              force_format;
static int              frame_count = 70;

static void errno_exit(const char *s)
{
        fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
        exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
}

static void process_image(const void *p, int size)
{
  static int i = 0;

  if (out_buf)
    fwrite(p, size, 1, stdout);

  fflush(stderr);
  fprintf(stderr, ".");
  fflush(stdout);
         
  i++;

  if (UDP_send (sock_udp, p, size) > 0)
    printf ("send frame %d\n", i);
}

static int read_frame(void)
{
        struct v4l2_buffer buf;
        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("read");
                        }
                }

                process_image(buffers[0].start, buffers[0].length);
                break;

        case IO_METHOD_MMAP:
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;

                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("VIDIOC_DQBUF");
                        }
                }

                assert(buf.index < n_buffers);

                process_image(buffers[buf.index].start, buf.bytesused);

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
                break;

        case IO_METHOD_USERPTR:
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_USERPTR;

                if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                        case EAGAIN:
                                return 0;

                        case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                        default:
                                errno_exit("VIDIOC_DQBUF");
                        }
                }

                for (i = 0; i < n_buffers; ++i)
                        if (buf.m.userptr == (unsigned long)buffers[i].start
                            && buf.length == buffers[i].length)
                                break;

                assert(i < n_buffers);

                process_image((void *)buf.m.userptr, buf.bytesused);

                if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                        errno_exit("VIDIOC_QBUF");
                break;
        }

        return 1;
}

static void mainloop(void)
{
        unsigned int count;
        unsigned int loopIsInfinite = 0;

        if (frame_count == 0) loopIsInfinite = 1;

        count = frame_count;

        //while (count-- > 0) {
        while ((count-- > 0) || loopIsInfinite) {
          if (quit) break;
                for (;;) {
                        fd_set fds;
                        struct timeval tv;
                        int r;

                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);

                        if (-1 == r) {
                                if (EINTR == errno)
                                        continue;
                                errno_exit("select");
                        }

                        if (0 == r) {
                                fprintf(stderr, "select timeout\n");
                                exit(EXIT_FAILURE);
                        }

                        if (read_frame())
                                break;
                        /* EAGAIN - continue select loop. */
                }
        }
}

static void stop_capturing(void)
{
        enum v4l2_buf_type type;

        switch (io) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
                        errno_exit("VIDIOC_STREAMOFF");
                break;
        }
}

static void start_capturing(void)
{
        unsigned int i;
        enum v4l2_buf_type type;

        switch (io) {
        case IO_METHOD_READ:
                /* Nothing to do. */
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_MMAP;
                        buf.index = i;

                        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                                errno_exit("VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                        errno_exit("VIDIOC_STREAMON");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_USERPTR;
                        buf.index = i;
                        buf.m.userptr = (unsigned long)buffers[i].start;
                        buf.length = buffers[i].length;

                        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
                                errno_exit("VIDIOC_QBUF");
                }
                type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
                        errno_exit("VIDIOC_STREAMON");
                break;
        }
}

static void uninit_device(void)
{
        unsigned int i;

        switch (io) {
        case IO_METHOD_READ:
                free(buffers[0].start);
                break;

        case IO_METHOD_MMAP:
                for (i = 0; i < n_buffers; ++i)
                        if (-1 == munmap(buffers[i].start, buffers[i].length))
                                errno_exit("munmap");
                break;

        case IO_METHOD_USERPTR:
                for (i = 0; i < n_buffers; ++i)
                        free(buffers[i].start);
                break;
        }

        free(buffers);
}

static void init_read(unsigned int buffer_size)
{
        buffers = calloc(1, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        buffers[0].length = buffer_size;
        buffers[0].start = malloc(buffer_size);

        if (!buffers[0].start) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }
}

static void init_mmap(void)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count = 4;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "memory mapping\n", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        buffers = calloc(req.count, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
                        errno_exit("VIDIOC_QUERYBUF");

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start =
                        mmap(NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start)
                        errno_exit("mmap");
        }
}

static void init_userp(unsigned int buffer_size)
{
        struct v4l2_requestbuffers req;

        CLEAR(req);

        req.count  = 4;
        req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_USERPTR;

        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s does not support "
                                 "user pointer i/o\n", dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_REQBUFS");
                }
        }

        buffers = calloc(4, sizeof(*buffers));

        if (!buffers) {
                fprintf(stderr, "Out of memory\n");
                exit(EXIT_FAILURE);
        }

        for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
                buffers[n_buffers].length = buffer_size;
                buffers[n_buffers].start = malloc(buffer_size);

                if (!buffers[n_buffers].start) {
                        fprintf(stderr, "Out of memory\n");
                        exit(EXIT_FAILURE);
                }
        }
}

static void init_device(void)
{
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        unsigned int min;

        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                        fprintf(stderr, "%s is no V4L2 device\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                } else {
                        errno_exit("VIDIOC_QUERYCAP");
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                         dev_name);
                exit(EXIT_FAILURE);
        }

        switch (io) {
        case IO_METHOD_READ:
                if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                        fprintf(stderr, "%s does not support read i/o\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                }
                break;

        case IO_METHOD_MMAP:
        case IO_METHOD_USERPTR:
                if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                        fprintf(stderr, "%s does not support streaming i/o\n",
                                 dev_name);
                        exit(EXIT_FAILURE);
                }
                break;
        }


        /* Select video input, video standard and tune here. */


        CLEAR(cropcap);

        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
                        switch (errno) {
                        case EINVAL:
                                /* Cropping not supported. */
                                break;
                        default:
                                /* Errors ignored. */
                                break;
                        }
                }
        } else {
                /* Errors ignored. */
        }


        CLEAR(fmt);

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (force_format) {
                fmt.fmt.pix.width       = 640;
                fmt.fmt.pix.height      = 480;
                fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
                fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

                if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
                        errno_exit("VIDIOC_S_FMT");

                /* Note VIDIOC_S_FMT may change width and height. */
        } else {
                /* Preserve original settings as set by v4l2-ctl for example */
                if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
                        errno_exit("VIDIOC_G_FMT");
        }

        /* Buggy driver paranoia. */
        min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;
        min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

        switch (io) {
        case IO_METHOD_READ:
                init_read(fmt.fmt.pix.sizeimage);
                break;

        case IO_METHOD_MMAP:
                init_mmap();
                break;

        case IO_METHOD_USERPTR:
                init_userp(fmt.fmt.pix.sizeimage);
                break;
        }
}

static void close_device(void)
{
        if (-1 == close(fd))
                errno_exit("close");

        fd = -1;
}

static void open_device(void)
{
        struct stat st;

        if (-1 == stat(dev_name, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", dev_name);
                exit(EXIT_FAILURE);
        }

        fd = open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                         dev_name, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

static void usage(FILE *fp, int argc, char **argv)
{
        fprintf(fp,
                 "Usage: %s [options]\n\n"
                 "Version 1.3\n"
                 "Options:\n"
                 "-d | --device name   Video device name [%s]\n"
                 "-h | --help          Print this message\n"
                 "-m | --mmap          Use memory mapped buffers [default]\n"
                 "-r | --read          Use read() calls\n"
                 "-u | --userp         Use application allocated buffers\n"
                 "-o | --output        Outputs stream to stdout\n"
                 "-f | --format        Force format to 640x480 YUYV\n"
                 "-c | --count         Number of frames to grab [%i]\n"
                 "",
                 argv[0], dev_name, frame_count);
}

static const char short_options[] = "d:hmruofc:a:p:";

static const struct option
long_options[] = {
        { "device", required_argument, NULL, 'd' },
        { "help",   no_argument,       NULL, 'h' },
        { "mmap",   no_argument,       NULL, 'm' },
        { "read",   no_argument,       NULL, 'r' },
        { "userp",  no_argument,       NULL, 'u' },
        { "output", no_argument,       NULL, 'o' },
        { "format", no_argument,       NULL, 'f' },
        { "count",  required_argument, NULL, 'c' },
        { "ip",     required_argument, NULL, 'a' },
        { "port",   required_argument, NULL, 'p' },
        { 0, 0, 0, 0 }
};

static void echo(char *str, char *file)
{
  int fd = open(file, O_WRONLY);
  if(fd < 0)
  {
    printf("%s:open error.\n", file);
    exit(-1);
  }
  write(fd, str, strlen(str));
  close(fd);
}

void *thread_control (void *vargs)
{
  const int PWM_RANGE = 396000; //(ns)
  const float WHEEL_R = 0.05; //(m)
  const float TMR_LEN = 0.10; //(m)
  const float ROOT_3 = 1.73205081;

  char *path = "/sys/devices/ocp.3";
  char *pwm[3] = {
    "pwm_test_P9_22.15",
    "pwm_test_P9_14.16",
    "pwm_test_P8_19.17"
  };

  int pwm_dead_band[3][2] = {
    51000, 99000,  //PWM0A dead-band upper, lower
    46000, 104000, //PWM1A dead-band upper, lower
    47000, 97000   //PWM2A dead-band upper, lower
  };

  float gain_v = 0.5;
  float gain_w = 1.0;

  int fd_PWM[3];
  int fd_light[2];

  char file[256];
  
  int pwm_duty[3];
  char pwm_duty_str[3][16];

  char light_str[2][8];

  char str_ctrl[128];
  int n;
  int i;
 
  char *token;
  int command_id;
  int sec;
  int msec; 
  int LL, RL;
  float vx, vy, wr;

  /* Check if PWMs are acquired. */
  for(i = 0; i < 3; i++)
  {
    sprintf(file, "%s/%s", path, pwm[i]);
    if(access(file, 0) < 0)
    {
      printf("%s not exist.\n",file);
      return -1;
    }
  }

  /* Init PWMs via sysfs */
  for(i = 0; i < 3; i++)
  {
    sprintf(file, "%s/%s/%s", path, pwm[i], "run");
    echo("0", file);
    sprintf(file, "%s/%s/%s", path, pwm[i], "period");
    echo("3000000", file);
    sprintf(file, "%s/%s/%s", path, pwm[i], "duty");
    echo("1500000", file);
    sprintf(file, "%s/%s/%s", path, pwm[i], "run");
    echo("1", file);

    sprintf(file, "%s/%s/%s", path, pwm[i], "duty");
    fd_PWM[i] = open(file, O_RDWR);
    if(fd_PWM[i] < 0)
    {
      printf("can not open PWM%dA's duty file.\n", i);
      return -1;
    }
  }

  /* Init Light */
  echo("30", "/sys/class/gpio/export");
  echo("31", "/sys/class/gpio/export");
  echo("out", "/sys/class/gpio/gpio30/direction");
  echo("out", "/sys/class/gpio/gpio31/direction");
  echo("0", "/sys/class/gpio/gpio30/value");
  echo("0", "/sys/class/gpio/gpio31/value");

  fd_light[0] = open("/sys/class/gpio/gpio30/value", O_WRONLY);
  fd_light[1] = open("/sys/class/gpio/gpio31/value", O_WRONLY);
 
  for (;;) {
    n = UDP_recv (sock_udp, str_ctrl, sizeof str_ctrl);

    if (n > 0) {
      printf("\t\t\t\t\t%s\n", str_ctrl);

      if (strcmp(str_ctrl, "End") == 0) break;

      token = strtok(str_ctrl, " ");
      command_id = atoi(token);
      token = strtok(NULL, ".");
      sec = atoi(token);
      token = strtok(NULL, " ");
      msec = atoi(token);
      token = strtok(NULL, " ");
      vx = (float)atoi(token);
      token = strtok(NULL, " ");
      vy = (float)atoi(token);
      token = strtok(NULL, " ");
      wr = (float)atoi(token);
      token = strtok(NULL, " ");
      LL = atoi(token);
      token = strtok(NULL, " ");
      RL = atoi(token);

      vx = PWM_RANGE * vx / 100.0 * gain_v;
      vy = PWM_RANGE * vy / 100.0 * gain_v;
      wr = PWM_RANGE * wr / 100.0 * gain_w;

      pwm_duty[0] = (-2.0/3.0*vy) + (1.0/3.0*TMR_LEN*wr);
      pwm_duty[1] = (-1.0/ROOT_3*vx) + (1.0/3.0*vy) + (1.0/3.0*TMR_LEN*wr);
      pwm_duty[2] = (1.0/ROOT_3*vx) + (1.0/3.0*vy) + (1.0/3.0*TMR_LEN*wr);

      pwm_duty[0] /= WHEEL_R;
      pwm_duty[1] /= WHEEL_R;
      pwm_duty[2] /= WHEEL_R;

      /* scaling */
      pwm_duty[0] /= (1.0/ROOT_3 + 2 + TMR_LEN)/WHEEL_R;
      pwm_duty[1] /= (1.0/ROOT_3 + 2 + TMR_LEN)/WHEEL_R;
      pwm_duty[2] /= (1.0/ROOT_3 + 2 + TMR_LEN)/WHEEL_R;

      if(pwm_duty[0] > 0) pwm_duty[0] += pwm_dead_band[0][0];
      if(pwm_duty[0] < 0) pwm_duty[0] -= pwm_dead_band[0][1];
      if(pwm_duty[1] > 0) pwm_duty[1] += pwm_dead_band[1][0];
      if(pwm_duty[1] < 0) pwm_duty[1] -= pwm_dead_band[1][1];
      if(pwm_duty[2] > 0) pwm_duty[2] += pwm_dead_band[2][0];
      if(pwm_duty[2] < 0) pwm_duty[2] -= pwm_dead_band[2][1];

      pwm_duty[0] += 1500000; //ns
      pwm_duty[1] += 1500000; //ns
      pwm_duty[2] += 1500000; //ns
      
      sprintf(pwm_duty_str[0], "%d", pwm_duty[0]);
      sprintf(pwm_duty_str[1], "%d", pwm_duty[1]);
      sprintf(pwm_duty_str[2], "%d", pwm_duty[2]);

      sprintf(light_str[0], "%d", LL);
      sprintf(light_str[1], "%d", RL);

      write(fd_PWM[0], pwm_duty_str[0], strlen(pwm_duty_str[0]));
      write(fd_PWM[1], pwm_duty_str[1], strlen(pwm_duty_str[1]));
      write(fd_PWM[2], pwm_duty_str[2], strlen(pwm_duty_str[2]));

      write(fd_light[0], light_str[0], strlen(light_str[0]));
      write(fd_light[1], light_str[1], strlen(light_str[1]));
    }
  }

  /* Stop PWM */
  for(i = 0; i < 3; i++)
  {
    sprintf(file, "%s/%s/%s", path, pwm[i], "run");
    printf("run out %s\n", file);
    echo("0", file);
    close(fd_PWM[i]);
  }

  close(fd_light[0]);
  close(fd_light[1]);

  /* unexport Light */
  echo("0", "/sys/class/gpio/gpio30/value");
  echo("0", "/sys/class/gpio/gpio31/value");
  echo("in", "/sys/class/gpio/gpio30/direction");
  echo("in", "/sys/class/gpio/gpio31/direction");
  echo("30", "/sys/class/gpio/unexport");
  echo("31", "/sys/class/gpio/unexport");

  
  quit = true;

  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t tid;
  int port;
  char str_ip[32];

        dev_name = "/dev/video0";

        sock_udp = UDP_new_socket ();
        UDP_socket_init (sock_udp);

        for (;;) {
                int idx;
                int c;

                c = getopt_long(argc, argv,
                                short_options, long_options, &idx);

                if (-1 == c)
                        break;

                switch (c) {
                case 0: /* getopt_long() flag */
                        break;

                case 'd':
                        dev_name = optarg;
                        break;

                case 'h':
                        usage(stdout, argc, argv);
                        exit(EXIT_SUCCESS);

                case 'm':
                        io = IO_METHOD_MMAP;
                        break;

                case 'r':
                        io = IO_METHOD_READ;
                        break;

                case 'u':
                        io = IO_METHOD_USERPTR;
                        break;

                case 'o':
                        out_buf++;
                        break;

                case 'f':
                        force_format++;
                        break;

                case 'c':
                        errno = 0;
                        frame_count = strtol(optarg, NULL, 0);
                        if (errno)
                                errno_exit(optarg);
                        break;
                        
                case 'a':
                        strncpy (str_ip, optarg, sizeof str_ip);
                        printf ("addr: %s\n", optarg);
                        break;

                case 'p':
                        port = atoi(optarg);
                        printf ("port: %d\n", atoi (optarg));
                        break;

                default:
                        usage(stderr, argc, argv);
                        exit(EXIT_FAILURE);
                }
        }

        UDP_set_client (sock_udp, port, str_ip);

        pthread_create (&tid, NULL, thread_control, NULL);

        frame_count = 0;

        open_device();
        init_device();
        start_capturing();
        mainloop();
        stop_capturing();
        uninit_device();
        close_device();
        fprintf(stderr, "\n");

        pthread_join (tid, NULL);
        
        return 0;
}

