#!/bin/bash


#QUERY_STRING="filename=Tazzina.stl&code=ABC&Submit=Submit+Query"


############### PARSE ARGS ################


STLPATH=/var/www/stl

FILENAME=$( echo $QUERY_STRING | sed 's/.*filename=\([^&]*\).*/\1/' )
CODE=$( echo $QUERY_STRING | sed 's/.*code=\([^&]*\).*/\1/' )
FILE=$STLPATH/$FILENAME


if [ ! -f $FILE ]; then
	echo "Content-type: text/html"
	echo
	echo "<p>Error file $FILE not found</p>"
	exit 
fi


############## RUN OPENSCAD ###############


export OPENSCADPATH=/usr/lib/cgi-bin

TMP_OUT=$(tempfile).stl
trap "rm -f $TMP_OUT" EXIT

TMP_SCAD=$(tempfile).scad
trap "rm -f $TMP_SCAD" EXIT

echo "
use <Write.scad>

difference() {
    import_stl(\"$FILE\", 4);
    translate([0, 0, 5])
    write(\"$CODE\",t=10,h=7,center=true);
}
" > $TMP_SCAD

openscad $TMP_SCAD -o $TMP_OUT


########### RETURN FILE TO USER ########

OUTNAME=$(basename $FILENAME .stl)_${CODE}.stl

echo "Content-type: application/sla"
echo "Content-Disposition: inline; filename=\"$OUTNAME\""
echo
cat $TMP_OUT

