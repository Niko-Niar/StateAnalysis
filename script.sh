#!/bin/bash
cd 'C:\mosquitto\'
for (( a = 1; a <= 3; a++ ))
do
run mosquitto_pub.exe -p 1883 -h localhost -t test -m 0
sleep 1
done
run mosquitto_pub.exe -p 1883 -h localhost -t test -m 'Data" "recording" "started'
list=(12000 32000 34000 34000 36000 36000 36000 34000 34000 12000)
sleep 1
for (( b = 0; b <= 9; b++ ))
do
((i = ${list[b]}))
run mosquitto_pub.exe -p 1883 -h localhost -t test -m $i
sleep 1
done
for (( c = 1; c <= 2; c++ ))
do
run mosquitto_pub.exe -p 1883 -h localhost -t test -m 0
sleep 1
done
run mosquitto_pub.exe -p 1883 -h localhost -t test -m 'Data" "recording" "is" "over'
run mosquitto_pub.exe -p 1883 -h localhost -t test -m 'File" "link:" "C:\11618833245_100000.log'