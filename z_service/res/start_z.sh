#!/bin/bash

sudo service wicd stop
sudo sysctl -w net/ipv4/ip_forward=1
sudo iptables-restore < /home/pi/projects/res/iptables_fwd.save
sudo /usr/local/etc/init.d/hostapd start
sudo /etc/init.d/dnsmasq restart
#sudo /etc/init.d/avahi-daemon restart

/home/pi/projects/bin/z_server &

exit 0



