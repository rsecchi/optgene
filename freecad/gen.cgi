#!/bin/bash

############### PARSE ARGS ################

set -a

STLPATH=/var/www/stl
CGIPATH=/usr/lib/cgi-bin
OUTPATH=${STLPATH}/
FILENAME=$( echo $QUERY_STRING | sed 's/.*filename=\(.*\)$/\1/' )
DESIGNER=$( echo $QUERY_STRING | sed 's/.*designer=\(.*\)&.*$/\1/' )


FILE=$STLPATH/$DESIGNER/$FILENAME


if [ ! -f $FILE ]; then
	echo "Content-type: text/html"
	echo
	echo "<p>Error: $FILE not found</p>"
	exit 
fi

CODE1=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 2 | head -n 1)
CODE2=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 3 | head -n 1)

OUTNAME=${DESIGNER}_$(basename $FILENAME .stl)_${CODE1}${CODE2}.stl

############## RUN OPENSCAD ###############

$CGIPATH/run_openscad.sh &




########### RETURN PAGE TO USER ########

echo "Content-type: text/html"
echo
echo "<p>Please wait!</p>"
echo "<p>The file name is ${OUTNAME}</p>"
echo "<p>The file will be placed in out_stl</p>"


