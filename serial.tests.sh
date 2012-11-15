#!/usr/bin/env sh
echo "######## Serial ########"
FILENAME=time_results-serial.txt
if [ $# -lt 2 ]
then
    echo "USAGE: `basename $0` [\"list of nx values\"]"
    WRONG_PARAMS=65
    exit $WRONG_PARAMS
fi
NX_LIST=$1
for NX in $NX_LIST
do
    echo "NX=$NX"
    COMMAND="./heat_serial $NX"
    /usr/bin/time -f "$NX %e" --output=time_output $COMMAND
    cat time_output | tee -a $FILENAME
done
