if [ $# -ne 1 ]
then
    echo "USAGE: `basename $0` filename.gif"
    WRONG_ARGS=65
    exit $WRONG_ARGS
fi
convert heatmaps_*.png $1
