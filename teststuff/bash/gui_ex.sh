#!/bin/bash

while sleep 0.1
do
  tput cup $((RANDOM%LINES)) $((RANDOM%COLUMNS))
  printf "*"
done
