#!/bin/bash

# set name for servo 0, servo 1, and servo 2
cd /sys/devices/ocp.3
read -r temp1 temp2 temp3 <<<$(find . -type d -name "pwm_test_p*")
# change names: ./pwm_test_p9_31 to pwm_test_p9_31
temp1=$(echo $temp1 | cut -d'/' -f 2)
temp2=$(echo $temp2 | cut -d'/' -f 2)
temp3=$(echo $temp3 | cut -d'/' -f 2)
# sort them, so the resulting temp_str is EXACTLY "pwm_test_p8_19.xx pwm_test_p9_14.yy pwm_test_p9_31.zz"
temp_str="$temp1 $temp2 $temp3"
temp_str=$(for i in $str; do
    echo $i
done | sort)
# now assign each of them to servo2, servo1, and servo0
read -r servo2 servo1 servo0 <<<$temp_str

# before we begin, set period for all servo
# for PWM0A:
echo 0 > "/sys/devices/ocp.3/$servo0/run"
echo 3000000 > "/sys/devices/ocp.3/$servo0/period"
echo 1 > "/sys/devices/ocp.3/$servo0/run"
# for PWM1A: 
echo 0 > "/sys/devices/ocp.3/$servo1/run"
echo 3000000 > "/sys/devices/ocp.3/$servo1/period"
echo 1 > "/sys/devices/ocp.3/$servo1/run"
# for PWM2A:
echo 0 > "/sys/devices/ocp.3/$servo2/run"
echo 3000000 > "/sys/devices/ocp.3/$servo2/period"
echo 1 > "/sys/devices/ocp.3/$servo2/run"

# loop (for PWM0A, PWM1A, PWM2A)
while true;
do
    # FOR PWM0A
    read -p "Enter duty for PWM0A (in ns): " duty
    if [ $duty -lt 0 -o $duty -gt 3000000] # here we also exit if duty is greater than 3ms
        # if duty is negative, stop and exit
        then break
    fi
    # assign duty value for PWM0A
    # since period is staying the same, no need for disable-set period-enable
    echo $duty > "/sys/devices/ocp.3/$servo0/duty"

    # FOR PWM1A
    read -p "Enter duty for PWM1A (in ns): " duty
    if [ $duty -lt 0 -o $duty -gt 3000000] # here we also exit if duty is greater than 3ms
        # if duty is negative, stop and exit
        then break
    fi
    # assign duty value for PWM1A
    # since period is staying the same, no need for disable-set period-enable
    echo $duty > "/sys/devices/ocp.3/$servo1/duty"

    # FOR PWM2A
    read -p "Enter duty for PWM2A (in ns): " duty
    if [ $duty -lt 0 -o $duty -gt 3000000] # here we also exit if duty is greater than 3ms
        # if duty is negative, stop and exit
        then break
    fi
    # assign duty value for PWM2A
    # since period is staying the same, no need for disable-set period-enable
    echo $duty > "/sys/devices/ocp.3/$servo2/duty"
done

# before we exit, disable the PWMs
echo 0 > "/sys/devices/ocp.3/$servo0/run"
echo 0 > "/sys/devices/ocp.3/$servo1/run"
echo 0 > "/sys/devices/ocp.3/$servo2/run"