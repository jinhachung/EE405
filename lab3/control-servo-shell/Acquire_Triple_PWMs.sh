#!/bin/bash

# 1. acquire overall PWM module
# check for bone_capemgr.N in /sys/devices
cd /sys/devices
# look for directory with format bone_capemgr.N, set it to directoryname
directoryname=$(find . -type d -name "bone_capemgr.*")
# change ./bone_capemgr.N to bone_capemgr.N
directoryname=$(echo $directoryname | cut -d'/' -f 2)
cd $directoryname
# now we are at /sys/devices/bone_capemgr.N
# add overall PWM module, "./slots" == "/sys/devices/bone_capemgr.N/slots"
echo am33xx_pwm > ./slots

# 2. acquire individual PWM
# add PWM0A
echo bone_pwm_P9_31 > ./slots
# add PWM1A
echo bone_pwm_P9_14 > ./slots
# add PWM2A
echo bone_pwm_P8_19 > ./slots

# 4. confirm by listing suitable directory
ls

