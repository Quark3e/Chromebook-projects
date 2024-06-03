#!/bin/bash

LIST="https://www.youtube.com/laylist?list=OLAK5uy_lTb1q7avK0lLJcQrMKe1JY5Mki3eQ9zqI"
SOURCE="/playlist?list="
if echo "$LIST" | grep -q "$SOURCE"; then
  echo "matched";
else
  echo "no match";
fi
