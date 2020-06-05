#!/bin/sh

echo Run: $0

echo PWMSS
echo "am33xx_pwm > /sys/devices/bone_capemgr.9/slots"
echo am33xx_pwm > /sys/devices/bone_capemgr.9/slots

echo PWM0A
echo "bone_pwm_P9_22 > /sys/devices/bone_capemgr.9/slots"
echo bone_pwm_P9_22 > /sys/devices/bone_capemgr.9/slots

echo PWM1A
echo "bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots"
echo bone_pwm_P9_14 > /sys/devices/bone_capemgr.9/slots

echo PWM2A
echo "bone_pwm_P8_19 > /sys/devices/bone_capemgr.9/slots"
echo bone_pwm_P8_19 > /sys/devices/bone_capemgr.9/slots

echo List directory
find /sys/devices/ocp.3/ -name '*epwmss'
find /sys/devices/ocp.3/ -name 'pwm_test*'
