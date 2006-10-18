#!/bin/sh

HOME=/scratch/mmbruin
IMGDIR=$HOME/chaos3/imgs
BMPDIR=$HOME/chaos3/bmps

for f in $IMGDIR/*.tif ; do
	convert -verbose -contrast -depth 24 -type truecolor -resize '50%' $f $BMPDIR/`basename $f tif`bmp
done

ffmpeg -r 30 -b 3600 -i $BMPDIR/%06d.bmp $HOME/test.mp4

