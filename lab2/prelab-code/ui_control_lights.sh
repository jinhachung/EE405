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
    if [ $light_id -lt 1 -o $light_id -gt 2 ]
        then break
    fi
    # C. check valid on_off_str (either "on" or "off")
    if [ $on_off_str != "on" -a $on_off_str != "off" ]
        then break
    fi
    # if program reaches here, light_id on_off_str are valid inputs
    # D. action for correct input
    # set file depending on light_id
    if [ $light_id == 1 ]
        then file_to_access="sys/class/gpio/gpio30/value"
    else
        file_to_access="sys/class/gpio/gpio31/value"
    fi
    # on -> 1, off -> 0
    if [ $on_off_str == "on" ]
        then on_off_int=1
    else
        on_off_int=0
    fi
    # set on/off for the chosen file
    echo $on_off_int > $file_to_access
done
# 8. set directions as input
echo in > /sys/class/gpio/gpio30/direction
echo in > /sys/class/gpio/gpio31/direction
# 9. unexport - release access permission for GPIO 30 & 31
echo 30 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio30
echo 31 > /sys/class/gpio/unexport # this should remove directory /sys/class/gpio/gpio31
