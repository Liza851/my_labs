#!/bin/bash

HOST=127.0.0.1
PORT=6033
LOGIN=stud
PASSWORD=stud
DATABASE=stud

AMQP_HOST=127.0.0.1
AMQP_PORT=5672
AMQP_LOGIN=guest
AMQP_PASSWORD=guest
AMQP_TOPIC=hello
AMQP_URL=/

BUILD_DIR=build
EXECUTABLE_FILE=$BUILD_DIR/poco-http-server-lab4

mkdir -p build

cd build
cmake ..
make

cd ..

./$EXECUTABLE_FILE \
    --host=$HOST \
    --port=$PORT \
    --login=$LOGIN \
    --password=$PASSWORD \
    --database=$DATABASE \
    --amqp_host=$AMQP_HOST \
    --amqp_port=$AMQP_PORT \
    --amqp_login=$AMQP_LOGIN \
    --amqp_password=$AMQP_PASSWORD \
    --topic=$AMQP_TOPIC \
    --url=$AMQP_URL
