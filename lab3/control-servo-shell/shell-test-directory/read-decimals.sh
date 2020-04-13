#!/bin/bash

read -p "Enter number in decimal: " x

if [ x -lt 0 ]
    then echo "number smaller than 0"
else if [ x -gt 1 ]
    then echo "number greater than 1"
else
    echo "number in range [0, 1]"
fi

echo x
