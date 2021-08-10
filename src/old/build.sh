#!/usr/bin/env bash
set -e

# gcc -o bin/consumer consumer.c -lpthread
# gcc -o bin/producer producer.c -lpthread

gcc server.c logging.c -o bin/server -lrt
gcc client.c logging.c -o bin/client -lrt