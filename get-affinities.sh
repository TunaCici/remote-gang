#!/usr/bin/env bash

if [ $# -eq 0 ]; then
    echo "Usage ./get-affinities.sh <PID>"
    exit 1
fi

pid=$1

name=$(cat /proc/$PID/comm)
echo "[x] ${pid} (${name})"

for tid in $(ls /proc/$PID/task/); do
    status="/proc/$PID/task/$tid/status"

    if [ -e "$status_file" ]; then
        name=$(cat /proc/${pid}/task/${tid}/comm)
        affinity=$(cat ${status} | grep -w "Cpus_allowed_list" | grep -oE '[0-9,-]+')
    
    echo "[x] \-- ${tid} ${name}, Core Affinity: ${affinity}"
    fi
done

