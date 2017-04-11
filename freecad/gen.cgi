#!/bin/bash

############### PARSE ARGS ################

lockfile -r 22 /tmp/wedo.lock

set -a

STLPATH=/var/www/stl/
CGIPATH=/usr/lib/cgi-bin
OUTPATH=${STLPATH}/
#FILENAME=$( echo $QUERY_STRING | sed 's/.*filename=\(.*\)$/\1/' )
#DESIGNER=$( echo $QUERY_STRING | sed 's/.*designer=\(.*\)&.*$/\1/' )

#ORDINE=$( echo $QUERY_STRING | sed 's/.*ordine=\(.*\)$/\1/' )
#PRODOTTO=$( echo $QUERY_STRING | sed 's/.*prodotto=\(.*\)$/\1/' )
#CODICE=$( echo $QUERY_STRING | sed 's/.*codice=\(.*\)&.*$/\1/' )

saveIFS=$IFS
IFS='=&'
parm=($QUERY_STRING)
IFS=$saveIFS

CODICE=${parm[1]}
ORDINE=${parm[3]}
PRODOTTO=${parm[5]}

DESIGNER=repos
FILENAME=$(echo $CODICE).stl

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

CODE1=$(/usr/local/bin/stl_makecode 2)
CODE2=$(/usr/local/bin/stl_makecode 3)

OUTNAME=$(basename $FILENAME .stl)_${CODE1}${CODE2}.stl

############## RUN OPENSCAD ###############

$CGIPATH/run_openscad.sh &




########### RETURN PAGE TO USER ########

echo "Content-type: text/html"
echo
cat /var/www/html/genera_list.html
echo "<p>Please wait!</p>"
echo "<p>The file name is ${OUTNAME}</p>"
echo "<p>The file will be placed in "
echo "<a href=\"http://137.74.161.44/out_stl\">"
echo "out_stl</a></p>"
echo "</body>"
echo "</html>"
