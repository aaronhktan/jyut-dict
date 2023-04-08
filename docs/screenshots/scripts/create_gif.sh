#!/bin/sh
# Adapted from http://blog.pkh.me/p/21-high-quality-gif-with-ffmpeg.html
# Converts a series of png files (named 1.png 2.png ... n.png) in a folder $1
# to a movie $2, which is then converted to a gif $3.

palette="/tmp/palette.png"

filters="fps=1"

find $1 -name "[0-9]*.png" -print0 | while read -d $'\0' file
do
  echo "Converting $file to remove transparent background..."
  convert $file -background white -alpha remove $file
done

ffmpeg -framerate 1 -i $1/%d.png -c:v libx264 -r 30 -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" -pix_fmt yuv420p $2
ffmpeg -v warning -i $2 -vf "$filters,palettegen" -y $palette
ffmpeg -v warning -i $2 -i $palette -lavfi "$filters [x]; [x][1:v] paletteuse" -y $3