#!/bin/sh

mkdir -p '/var/internet_usage/'
cp 'parser.py' '/var/internet_usage/'
chmod -R 0777 '/var/internet_usage/'

cp 'ltm.service' '/etc/systemd/system/'
systemctl start ltm.service
