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
start=$(date +%s.%N);
# this is displayed in "seconds.nanoseconds" form, so we need to multiply by 10E9
# to get time in nanoseconds
start=$(($start * 10));
# 5. finite loop many times

    # A. turn on light 1
    
    # B. turn on light 2
    
    # C. turn off light 1
    
    # D. turn off light 2
    
# 6. get end time (ns)
end=""
# 7. echo end/start time
echo "start time: $start"
# 8. set direction as input
echo in > /sys/class/gpio/gpio30/direction
echo in > /sys/class/gpio/gpio31/direction
# 9. unexport - release access permission for GPIO 30 & GPIO 31
echo 30 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio30
echo 31 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio31
