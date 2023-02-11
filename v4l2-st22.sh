#!/bin/bash

#Run
#index=4 ; ./build/app/RxTxApp --config_file config/v4l2_rx_1port_1v.json --lcores 1,5 &
#index=5 ; ./build/app/RxTxApp --config_file config/v4l2_rx_1port_1v.json --lcores 1,5 --perf_ip "192.168.1.200" &
#index=6 ; ./build/app/RxTxApp --config_file config/v4l2_rx_1port_1v.json --lcores 1,5 --display &
index=7 ; ./build/app/RxTxApp --config_file config/v4l2_rx_1port_1v.json --lcores 1,5 --display --perf_ip "192.168.1.200" &

sleep 20

prog_pid=$(ps -aux | grep RxTxApp | grep -v "grep"| awk '{print $2}')
#ps -T -p $prog_pid
prog_pid_1=$(ps -T -p $prog_pid | grep RxTxApp | awk -v ind=$index '{ if(NR==(ind+0)) {print $2} }')
prog_pid_2=$(ps -T -p $prog_pid | grep RxTxApp | awk -v ind=$index '{ if(NR==(ind+1)) {print $2} }')
prog_pid_3=$(ps -T -p $prog_pid | grep RxTxApp | awk -v ind=$index '{ if(NR==(ind+2)) {print $2} }')
prog_pid_4=$(ps -T -p $prog_pid | grep RxTxApp | awk -v ind=$index '{ if(NR==(ind+3)) {print $2} }')
prog_pid_5=$(ps -T -p $prog_pid | grep RxTxApp | awk -v ind=$index '{ if(NR==(ind+4)) {print $2} }')

taskset -p 0x04 $prog_pid_1
taskset -p 0x08 $prog_pid_2
taskset -p 0x40 $prog_pid_3
taskset -p 0x80 $prog_pid_4
taskset -p 0x10 $prog_pid_5
