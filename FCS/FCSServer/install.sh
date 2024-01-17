#!/bin/sh
chmod +x ./fcsServer
cp -f ./fcsServer.service /etc/systemd/system/
cp -f ./fcsServer /usr/sbin/
systemctl enable fcsServer.service
systemctl start fcsServer.service
systemctl status fcsServer.service