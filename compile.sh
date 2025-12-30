#!/bin/bash

mkdir -p bin
gcc -g -Iinclude src/*.c -o ./bin/server
