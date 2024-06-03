#!/usr/bin/bash

#Script to bulk download pictures from fapeza

#URL requires first and second letter of URL
declare first=$(echo "$1" | cut -c1-1)
declare second=$(echo "$1" | cut -c2-2)

#declare iterator
declare -i i=1

#Check if <name> exists as folder, enter it, if not create and enter
if [ ! -d "$1" ]; then
  mkdir "$1"
  cd "$1"
else
  cd "$1"
fi

#build url and download, if successful increment
while wget -q https://fapeza.com/media/"$first"/"$second"/"$1"/1000/"$1"_"$(printf "%04d" $i)".jpg ; do
  echo "Downloaded picture "$i 
  ((i++))
done
((i--))
cd ..

echo $i" pictures found and downloaded, some might be missing"