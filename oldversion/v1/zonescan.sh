#!/bin/bash

# This script will help to determine what zone numbers control which
# lights on your AlienWare hardware.
#
# Version 3.

echo "Lights out!"
sudo alienfx -r
echo "Press enter key for next zone"
read
echo "ZONE 01 (press enter)"
sudo alienfx -r -v -c ffff1000000000000000
read
echo "ZONE 02 (press enter)"
sudo alienfx -r -v -c ffff0100000000000000
read
echo "ZONE 03 (press enter)"
sudo alienfx -r -v -c ffff0010000000000000
read
echo "ZONE 04 (press enter)"
sudo alienfx -r -v -c ffff0001000000000000
read
echo "ZONE 05 (press enter)"
sudo alienfx -r -v -c ffff0000100000000000
read
echo "ZONE 06 (press enter)"
sudo alienfx -r -v -c ffff0000010000000000
read
echo "ZONE 07 (press enter)"
sudo alienfx -r -v -c ffff0000001000000000
read
echo "ZONE 08 (press enter)"
sudo alienfx -r -v -c ffff0000000100000000
read
echo "ZONE 09 (press enter)"
sudo alienfx -r -v -c ffff0000000010000000
read
echo "ZONE 10 (press enter)"
sudo alienfx -r -v -c ffff0000000001000000
read
echo "ZONE 11 (press enter)"
sudo alienfx -r -v -c ffff0000000000100000
read
echo "ZONE 12 (press enter)"
sudo alienfx -r -v -c ffff0000000000010000
read
echo "ZONE 13 (press enter)"
sudo alienfx -r -v -c ffff0000000000001000
read
echo "ZONE 14 (press enter)"
sudo alienfx -r -v -c ffff0000000000000100
read
echo "ZONE 15 (press enter)"
sudo alienfx -r -v -c ffff0000000000000010
read
echo "ZONE 16 (press enter)"
sudo alienfx -r -v -c ffff0000000000000001
read
echo "Lights out!"
sudo alienfx -r
