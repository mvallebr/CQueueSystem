#!/bin/sh

kill -15 $(ps -e | grep pars | cut -f 1 -d ?)
