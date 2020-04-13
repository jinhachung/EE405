#!/bin/bash

# we don't know the X and N values for slot-N that they were created in /sys/devices/bone_capemgr.X
# find X (as we did in Acquire)
cd /sys/devices
# look for directory with format bone_capemgr.N, set it to directoryname
directoryname=$(find . -type d -name "bone_capemgr.*")
# change ./bone_capemgr.N to bone_capemgr.N
directoryname=$(echo $directoryname | cut -d'/' -f 2)
cd $directoryname
# NOW WE ARE LOCATED AT /sys/devices/bone_capemgr.N

# find the N and sort them
# if they are sorted to be sort-A, sort-B, sort-C, sort-D,
# the smallest "sort-A" is overall PWM and rest are individuals)
read -r one two three four <<<$(find . -type d -name "slot-*")
# change ./slot-N to slot-N
one=$(echo $one | cut -d'/' -f 2)
two=$(echo $two | cut -d'/' -f 2)
three=$(echo $three | cut -d'/' -f 2)
four=$(echo $four | cut -d'/' -f 2)
# sort them
temp_str="$one $two $three $four"
temp_str=$(for i in $str; do
    echo $i
done | sort)
read -r overall individual1 individual2 individual3 <<<$temp_str

# release individual PWM
# find out number for each individual slot
# they are all of form "slot-N", so the 5th index is the number -> use substring
num1=${individual1:5:5}
num2=${individual2:5:5}
num3=${individual3:5:5}
# for the three Ns, echo "-N" to the slot file
echo "-$num1" > "/sys/devices/$directoryname/slots"
echo "-$num2" > "/sys/devices/$directoryname/slots"
echo "-$num3" > "/sys/devices/$directoryname/slots"

# release overall PWMSS
# this part is similar to the other three
# note that this part doesn't actually work and we will need to call sudo reboot
num = ${overall:5:5}
echo "-$num" > "sys/devices/$directoryname/slots"
#sudo reboot