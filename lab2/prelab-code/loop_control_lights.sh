#!/bin/bash

# 0. print title
echo "========== shell script for loop - light control =========="
# 1. export - get access permission for GPIO 30 & GPIO 31
echo 30 > /sys/class/gpio/export # this should create directory /sys/class/gpio/gpio30
echo 31 > /sys/class/gpio/export # this should create directory /sys/class/gpio/gpio31
# 2. set directions of GPIO 30 & GPIO 31 as output
echo out > /sys/class/gpio/gpio30/direction
echo out > /sys/class/gpio/gpio31/direction
# 3. get start time (ns) 
start=$(date +%s%N)
# 5. finite loop many times -> 100 times
for i in {1..100}
do
    # A. turn on light 1
    echo 1 > /sys/class/leds/beaglebone\:green\:usr1
    # B. turn on light 2
    echo 1 > /sys/class/leds/beaglebone\:green\:usr2 
    # C. turn off light 1
    echo 0 > /sys/class/leds/beaglebone\:green\:usr1
    # D. turn off light 2
    echo 0 > /sys/class/leds/beaglebone\:green\:usr2 
done
# 6. get end time (ns)
end=$(date +%s%N)
# 7. echo end/start time
echo "======= looped 100 times to get ======="
echo "start time: $start ns"
echo "  end time: $end ns"
time_taken=$(($end - $start))
echo "time taken: $time_taken ns in 100 loops"
avg_time=$(($time_taken/100))
echo "average time: $avg_time ns per loop"
echo "======================================="
# 8. set direction as input
echo in > /sys/class/gpio/gpio30/direction
echo in > /sys/class/gpio/gpio31/direction
# 9. unexport - release access permission for GPIO 30 & GPIO 31
echo 30 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio30
echo 31 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio31
