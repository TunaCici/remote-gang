#!/usr/bin/env bash

# 1. CPU Test (Prime Numbers)
sysbench cpu --threads=4 --cpu-max-prime=65536 run | tee sysbench-cpu-out.txt

# 2. Memory Read Test
sysbench memory --threads=4 --memory-oper=read --memory-total-size=2048m run | tee sysbench-memr-out.txt

# 4. Memory Write Test
sysbench memory --threads=4 --memory-oper=write --memory-total-size=2048m run | tee sysbench-memw-out.txt

# 5. File Sequantial Read Test
sysbench fileio --threads=4 --file-test-num=1 --file-extra-flags=directio --file-io-mode=seqread --file-total-size=1024m run | tee sysbench-filer-out.txt

# 6. File Random R/W Test
sysbench fileio --threads=4 --file-test-num=1 --file-extra-flags=directio --file-io-mode=rndrw --file-total-size=512m run | tee sysbench-filerw-out.txt

