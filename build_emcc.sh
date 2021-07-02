#!/bin/bash

source ~/git/emsdk/emsdk_env.sh
make clean
CC=emcc LDFLAGS=-lm make
