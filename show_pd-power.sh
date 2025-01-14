#!/bin/zsh

ioreg -l | grep \ \"PowerOutDetails | sed -e 's/[ \|]//g'
