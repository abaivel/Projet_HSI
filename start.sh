#!/bin/bash

cleanup(){
    if [ -n "$DRIVER_PID" ]; then
        kill $DRIVER_PID 2>/dev/null
    fi
    cd docker
    docker-compose down
    cd ..
    make clean
    exit 0
}

trap cleanup SIGINT SIGTERM
./driver &
DRIVER_PID=$!
cd docker
docker-compose up -d
cd ..
sleep 2
make
sleep 2
./app

cleanup