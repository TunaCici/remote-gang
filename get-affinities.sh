#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage ./get-affinities.sh <PID>"
    exit 1
fi

pid=$1

if [ ! -d "/proc/${pid}" ]; then
    echo "Process with PID ${pid} does not exist."
    exit 1 
fi

name=$(cat /proc/${pid}/comm)
echo "[x] ${pid} (${name})"

for tid in $(ls /proc/${pid}/task/); do
    status="/proc/${pid}/task/${tid}/status"

    if [ -e "$status_file" ]; then
        name=$(cat /proc/${pid}/task/${tid}/comm)
        affinity=$(cat ${status} | grep -w "Cpus_allowed_list" | grep -oE '[0-9,-]+')
    
    echo "[x] \-- ${tid} ${name}, Core Affinity: ${affinity}"
    fi
done

