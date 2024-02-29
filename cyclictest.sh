#!/usr/bin/env bash

sudo cyclictest --mlockall --smp --priority=80 --interval=200 --distance=0 --duration=1s | ts -s '%M:%.S' >> /dev/shm/out.tmp
cat /dev/shm/out.tmp > cyclictest-out.txt

