#!/bin/bash
cd 'C:\mosquitto\'
for (( a = 1; a <= 3; a++ ))
do
((k = $a*1000))
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":0},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":'$k',\"type\":\"value_changed\"}'
sleep 1
done
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{},\"timestamp\":4000,\"action\":\"store_next_record\"}'
sleep 1
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":0},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":5000,\"type\":\"value_changed\"}'
list=(500 700 1000 1300 2200 2200 1400 1300 1000 700)
sleep 1
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":'500'},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":6000,\"type\":\"value_changed\"}'
sleep 1
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":'700'},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":7000,\"type\":\"value_changed\"}'
sleep 1
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":'1000'},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":8000,\"type\":\"value_changed\"}'
sleep 1
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":'1200'},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":9000,\"type\":\"value_changed\"}'
sleep 1
for (( b = 4; b <= 9; b++ ))
do
((i = ${list[b]}))
((j = $b*1000+6000))
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":'$i'},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":'$j',\"type\":\"value_changed\"}'
sleep 1
done
for (( c = 1; c <= 2; c++ ))
do
((s = $c*1000+15000))
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"direction\":0,\"max\":0,\"min\":0,\"value\":0},\"level\":0,\"sender\":\"600956b0d499f8577e496b52\",\"timestamp\":'$s',\"type\":\"value_changed\"}'
sleep 1
done
run mosquitto_pub.exe -p 1883 -h localhost -t test -m '{\"data\":{\"file\":\"C:\\Logs\\1.log\"},\"timestamp\":18000,\"action\":\"taho_record_ended\"}'