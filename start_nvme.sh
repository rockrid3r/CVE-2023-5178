#!/bin/bash

if [ "$EUID" -ne 0 ]
    then echo "run as root"
    exit 1
fi

modprobe nvmet
modprobe nvmet-tcp

mkdir /sys/kernel/config/nvmet/ports/1
cd /sys/kernel/config/nvmet/ports/1
echo 127.0.0.1 |sudo tee -a addr_traddr > /dev/null
echo tcp|sudo tee -a addr_trtype > /dev/null
echo 4420|sudo tee -a addr_trsvcid > /dev/null
echo ipv4|sudo tee -a addr_adrfam > /dev/null

cd /sys/kernel/config/nvmet/subsystems
mkdir test
cd test
echo -n 1 > /sys/kernel/config/nvmet/subsystems/test/attr_allow_any_host

cd namespaces ; mkdir 1; cd 1
echo -n /dev/sda > device_path # or /dev/nvme0n1
echo -n 1 > enable

ln -s /sys/kernel/config/nvmet/subsystems/test/ /sys/kernel/config/nvmet/ports/1/subsystems/test
