#!/bin/sh

v4l2-ctl --set-fmt-video=width=640,height=480,pixelformat=1
v4l2-ctl -V
