#!/bin/sh
. pon_entorno
./k
make
rm -f ../perrors/* ../poutput/* ../aerrors/* ../aoutput/*
./pars.exe
