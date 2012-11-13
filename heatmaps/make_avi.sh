#!/bin/bash
startdir=`pwd`
if [ `basename $startdir` = 'heatmaps' ]
then
    date >> /tmp/yup
else
    cd heatmaps
fi
mencoder 'mf://heatmap_t*.png' -mf type=png:fps=10 -ovc lavc -oac copy -o animation.avi
avconv -y -i animation.avi animation.mp4
rm animation.avi
#rm heatmap_t*.png
cd $startdir
