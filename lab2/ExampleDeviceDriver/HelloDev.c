/*  File HelloDev.c
 *  A simple character device with file system operations
 */

#include <linux/module.h>	/* Needed by all modules */
#include <linux/fs.h>

# define HELLO_MAJOR	234

static int debug_enable = 0;
module_param(debug_enable, int, 0);
MODULE_PARM_DESC(debug_enable, "Enable module debug mode.");

struct file_operations HelloDev_fops;

static int HelloDev_open(struct inode *inode, struct file *file)
{
	printk("HelloDev_open: successful\n");
	return 0;
}

static int HelloDev_release(struct inode *inode, struct file *file)
{
	printk("HelloDev_release: successful\n");
	return 0;
}

static ssize_t HelloDev_read(struct file *file, char *buf, size_t count,
	loff_t *ptr)
{
	printk("HelloDev_read: returning zero bytes\n");
	return 0;
}

static ssize_t HelloDev_write(struct file *file, const char *buf, size_t count,
	loff_t *ppos)
{
	printk("HelloDev_write: accepting zero bytes\n");
	return 0;
}

static long HelloDev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk("HelloDev_ioctl: cmd=%d, arg=%ld\n", cmd, arg);
	return 0;
}

static int __init HelloDev_init(void)
{
	int ret;
	printk("HelloDev Init - debug mode is %s\n",
		debug_enable ? "enabled" : "disabled" );
	ret = register_chrdev(HELLO_MAJOR, "hellodev", &HelloDev_fops);
	if (ret < 0) {
		printk("Error registering HelloDev fops\n");
		return ret;
	}
	printk("HelloDev: registered successfully!\n");

	/* Init processing here ...	*/

	return 0;
}

static void __exit HelloDev_exit(void)
{
	unregister_chrdev(HELLO_MAJOR, "hellodev");

	printk("Goodbye, HelloDev\n");
}

struct file_operations HelloDev_fops = {
	owner:		THIS_MODULE,
	read:		HelloDev_read,
	write:		HelloDev_write,
	compat_ioctl:	HelloDev_ioctl,	// ioctl --> compat_ioctl Changed 2.6.36
	open:		HelloDev_open,
	release:	HelloDev_release,
};

module_init(HelloDev_init);
module_exit(HelloDev_exit);

MODULE_AUTHOR("Christoper Hallinan");
MODULE_DESCRIPTION("HelloDev Example");
MODULE_LICENSE("GPL");


