#!/bin/bash

# 0. print title
echo "========== shell script for light control =========="
# 1. export - get permission for GPIO 30 & GPIO 31
echo 30 > /sys/class/gpio/export # this should create directory /sys/class/gpio/gpio30
echo 31 > /sys/class/gpio/export # this should create directory /sys/class/gpio/gpio31
# 2. set directions of GPIO 30 & GPIO 31 as output
echo out > /sys/class/gpio/gpio30/direction
echo out > /sys/class/gpio/gpio31/direction
# 5. user interface infinite loop
while true;
    do
    # in infinite loop, read user input and break if valid
    # A. get user input of light_id and on_off_str
    read -p "Enter light_id and on_off_str: " light_id on_off_str
    # B. check valid light_id (in range [1, 2])
    if [ $light_id -lt 1 -o $light_id -gt 2 ];
        then break
    fi
    # C. check valid on_off_str (either "on" or "off")
    if [ $on_off_str != "on" -a $on_off_str != "off" ];
        then break
    fi
    # if program reaches here, light_id on_off_str are valid inputs
    # D. action for correct input
    echo "light_id:$light_id and on_off_str:$on_off_str"
    echo "WHAT EXACTLY IS THE ACTION FOR CORRECT INPUT?"
done
# 8. set directions as input
echo in > /sys/class/gpio/gpio30/direction
echo in > /sys/class/gpio/gpio31/direction
# 9. unexport - release access permission for GPIO 30 & 31
echo 30 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio30
echo 31 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio31
