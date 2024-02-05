#!/bin/bash

if [ "$EUID" -ne 0 ]
    then echo "run as root"
    exit 1
fi

cat /sys/module/nvmet_tcp/sections/.text
cat /sys/module/nvmet/sections/.text
