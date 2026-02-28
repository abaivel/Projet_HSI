#!/bin/bash

cleanup(){
    if [ -n "$DRIVER_PID" ]; then
        kill $DRIVER_PID 2>/dev/null
    fi
    cd docker
    sudo docker-compose down > /dev/null 2>&1
    cd ..
    make clean > /dev/null 2>&1
    exit 0
}

trap cleanup SIGINT SIGTERM
./driver &
DRIVER_PID=$!
cd docker
sudo docker-compose up -d > /dev/null 2>&1
cd ..
sleep 2
make > /dev/null 2>&1
sleep 2
./app

cleanup