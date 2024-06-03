#!/bin/bash

url="$1"

SOURCE="/playlist?list="

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

if echo "$url" | grep -q "$SOURCE"; then
    echo "type: PLAYLIST"
    printf "\n ---getting videos... :read link: \"$url\"\n"
    yt-dlp --ppa "ThumbnailsConvertor+ffmpeg_o:-c:v png -vf crop='ih'" --embed-thumbnail\
 --replace-in-metadata "uploader" "(\- Topic)" ""\
 --parse-metadata "%(uploader|)s:%(meta_artist)s"\
 --parse-metadata "playlist_title:%(meta_album)s"\
 --parse-metadata "%(uploader|)s:%(album_artist)s"\
 --parse-metadata "playlist_index:%(track_number)s"\
 --embed-metadata -f "bestaudio[ext=m4a],bestaudio[ext=mp3]" $url
else
    echo "type: VIDEO"
    printf "\n ---getting video... :read link: \"$url\"\n"
    yt-dlp --ppa "ThumbnailsConvertor+ffmpeg_o:-c:v png -vf crop='ih'" --embed-thumbnail\
 --replace-in-metadata "uploader" "(\- Topic)" ""\
 --parse-metadata "%(uploader|)s:%(meta_artist)s"\
 --parse-metadata "%(title)s:%(album)s"\
 --parse-metadata "%(uploader|)s:%(album_artist)s"\
 --parse-metadata "playlist_index:%(track_number)s"\
 --embed-metadata -f "bestaudio[ext=m4a],bestaudio[ext=mp3]" $url
fi


printf "\n ---sending video...\n"
fileName="$(yt-dlp --get-filename --no-download-archive -f "bestaudio[ext=m4a],bestaudio[ext=mp3]" $url)"
printf "\n ---filename: \"$fileName\"\n"

IFS=$'\n'
for item in $fileName
do
    printf "\tfile: $item\n" 
    rsync -ah --info=progress2 -e "ssh -p 2224" "$SCRIPTPATH/$item" berkhme@192.168.1.124:/storage/emulated/0/Music_files/
    printf "\tmoving file to \"sent/\"\n"
    rm -r "$SCRIPTPATH/$item"
    #mv "$SCRIPTPATH/$item" sent/
done

echo "finished"
