#!/bin/bash

# 1. use GPIO 60
echo 60 > /sys/class/gpio/export
# 2. set GPIO 60 to output mode
echo out > /sys/class/gpio/gpio60/direction
# 3. set GPIO 60 to high state
echo 1 > /sys/class/gpio/gpio60/value
# 4. set GPIO 60 to low state
echo 0 > /sys/class/gpio/gpio60/value
# 5. when done, unexport GPIO 60
echo 60 > /sys/class/gpio/unexport



