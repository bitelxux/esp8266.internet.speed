#!/bin/bash

speedtest > /tmp/speedtest
download=$(grep Download /tmp/speedtest | awk '{print $3}')
upload=$(grep Upload /tmp/speedtest | awk '{print $3}')
echo $download
echo $upload 
