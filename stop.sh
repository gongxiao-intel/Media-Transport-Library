#!/bin/bash

prog_pid=$(ps -aux | grep RxTxApp |grep -v "grep"| awk '{print $2}')
kill -n 2 $prog_pid

