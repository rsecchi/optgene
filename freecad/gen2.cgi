#!/bin/bash

############### PARSE ARGS ################

set -a

STLPATH=/var/www/stl
CGIPATH=/usr/lib/cgi-bin
OUTPATH=${STLPATH}/
FILENAME=$( echo $QUERY_STRING | sed 's/.*filename=\([^&]*\).*/\1/' )
DESIGNER=$( echo $QUERY_STRING | sed 's/.*designer=\([^&]*\).*/\1/' )

CODE1=$( echo $QUERY_STRING | sed 's/.*code1=\([^&]*\).*/\1/' )
CODE2=$( echo $QUERY_STRING | sed 's/.*code2=\([^&]*\).*/\1/' )

X1=$( echo $QUERY_STRING | sed 's/.*X1=\([^&]*\).*/\1/' )
Y1=$( echo $QUERY_STRING | sed 's/.*Y1=\([^&]*\).*/\1/' )
X2=$( echo $QUERY_STRING | sed 's/.*X2=\([^&]*\).*/\1/' )
Y2=$( echo $QUERY_STRING | sed 's/.*Y2=\([^&]*\).*/\1/' )

bold=$( echo $QUERY_STRING | sed 's/.*bold=\([^&]*\).*/\1/' )
space=$( echo $QUERY_STRING | sed 's/.*space=\([^&]*\).*/\1/' )
high=$( echo $QUERY_STRING | sed 's/.*high=\([^&]*\).*/\1/' )

#echo "Content-type: text/html"
#echo
#echo "<p> "$QUERY_STRING" </p>"
#echo "<p> "$FILENAME" </p>"
#echo "<p> "$DESIGNER" </p>"
#
#echo "<p> "$CODE1" </p>"
#echo "<p> "$CODE2" </p>"
#echo "<p> "$X1" </p>"
#echo "<p> "$Y1" </p>"
#echo "<p> "$X2" </p>"
#echo "<p> "$Y2" </p>"
#echo "<p> "$bold" </p>"
#echo "<p> "$space" </p>"
#echo "<p> "$high" </p>"
#exit

FILE=$STLPATH/$DESIGNER/$FILENAME


if [ ! -f $FILE ]; then
	echo "Content-type: text/html"
	echo
	cat /var/www/html/genera_list.html
	echo "<p>Error: $FILE not found</p>"
	echo "</body>"
	echo "</html>"
	exit 
fi

#CODE1=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 2 | head -n 1)
#CODE2=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 3 | head -n 1)

#CODE1=$(/usr/local/bin/stl_makecode 2)
#CODE2=$(/usr/local/bin/stl_makecode 3)

OUTNAME=${DESIGNER}_$(basename $FILENAME .stl)_${CODE1}${CODE2}.stl

############## RUN OPENSCAD ###############

$CGIPATH/run_openscad2.sh &




########### RETURN PAGE TO USER ########

echo "Content-type: text/html"
echo
cat /var/www/html/genera_list.html
echo "<p>Please wait!</p>"
echo "<p>The file name is ${OUTNAME}</p>"
echo "<p>The file will be placed in "
echo "<a href=\"http://137.74.161.44/test_stl\">"
echo "out_stl</a></p>"
echo "</body>"
echo "</html>"

