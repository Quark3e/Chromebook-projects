#!/bin/bash



list="One\ntwo\nthree\nfour"

#Print the list with echo
echo -e "echo: \n$list"

#Set the field separator to new line
IFS=$'\n'

#Try to iterate over each line
echo "For loop:"
for item in $list
do
        echo "Item: $item"
done

#Output the variable to a file
echo -e $list > list.txt

#Try to iterate over each line from the cat command
echo "For loop over command output:"
for item in `cat list.txt`
do
        echo "Item: $item"
done


echo "--------new--------"

list="One\ntwo\nthree\nfour"
echo "$list"


list=$'One\ntwo\nthree\nfour'
echo "$list"

IFS=$'\n'

echo "-------"

while IFS= read -r line; do
    echo "... $line ..."
done <<< "$list" | sed "s/\"/'/"

echo "--------"

while IFS= read -r line || [[ -n $line ]]; do
    echo "--- $line ..."
done < <(printf '%s' "$list2")


echo "--------"

while IFS= read -r line; do
    echo "... $line ..."
done < <(printf '%s' "$list")


echo "--------new--------"

items="
one two three four
hello world
this should work just fine
"

IFS='
'
count=0
for item in $items
do
  count=$((count+1))
  echo $count $item
done
