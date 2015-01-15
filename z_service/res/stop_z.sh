#!/bin/bash

sudo /usr/local/etc/init.d/hostapd stop
sudo /etc/init.d/dnsmasq restart

sudo killall z_server

exit 0



