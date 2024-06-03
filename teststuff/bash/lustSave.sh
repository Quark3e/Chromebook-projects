#!/bin/bash

#gallery-dl
opt00="x.com"
opt01="instagram"
opt02="twitter"

#yt-dlp
opt10="youtube"
opt11="reddit"

#echo "FolderLustSave/"

outPath="FolderLustSave/"

# outPath="/storage/emulated/0/Lust/gallery-dl/"

urlCheck () {
    if echo "$1" | grep -q "$opt00" || echo "$1" | grep -q "$opt01" || echo "$1" | grep -q "$opt02" ; then
        echo "gallery-dl"
        gallery-dl --dest $outPath "$1"
    else
        echo "yt-dlp"
        yt-dlp --path $outPath "$1"
    fi
}

for i in $@
do
    echo $i
done

echo "gap"

for i in $@
do
    urlCheck $i
done
echo "done"
