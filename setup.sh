#!/bin/bash

pkill ptp4l
pkill phc2sys

service irqbalance stop

./script/nicctl.sh bind_pmd 0000:01:00.0

ptp4l -i eno1 -A -2 -m -s &> /tmp/ptp4l-eno1.log &

phc2sys -c CLOCK_REALTIME -s eno1 -O 0 -w -m &> /tmp/phc2sys-eno1.log &

