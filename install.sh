#!/usr/bin/env bash

# mcc -W lib:librrwm -T link:lib -d outlib cRRWM.m
BASE=$HOME/dev/pkgs/
LIB=librrwm
cp outlib/${LIB}.h $BASE/include/rrwm/
cp outlib/${LIB}.so $BASE/lib/
