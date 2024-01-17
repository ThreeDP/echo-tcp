#!/bin/sh

make
chmod +x server
./server $SERVER_ADDR $SERVER_PORT