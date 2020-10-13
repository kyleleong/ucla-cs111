#!/bin/sh

if [ "$#" -lt "2" ]; then
    hexdump -C
else
    hexdump -C $1
fi
