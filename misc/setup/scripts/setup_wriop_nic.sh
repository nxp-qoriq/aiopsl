#!/bin/sh

if [ $# != 3 ]; then
	echo "Usage: $0 <interface name> <MAC address> <IP address>"
	exit 1
fi

IF_NAME=$1
MAC_ADDR=$2
IP_ADDR=$3

sudo tunctl -u ${USER} -t ${IF_NAME}
sudo ip link set ${IF_NAME} address ${MAC_ADDR}
sudo ip addr add ${IP_ADDR}/24 dev ${IF_NAME}
sudo ip link set ${IF_NAME} up
sudo iptables -I INPUT -i ${IF_NAME} -j ACCEPT
