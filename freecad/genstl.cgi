#!/bin/bash


#QUERY_STRING="filename=Tazzina.stl&code=ABC&Submit=Submit+Query"


############### PARSE ARGS ################


STLPATH=/var/www/stl

FILENAME=$( echo $QUERY_STRING | sed 's/.*filename=\([^&]*\).*/\1/' )
CODE1=$( echo $QUERY_STRING | sed 's/.*code1=\([^&]*\).*/\1/' )
CODE2=$( echo $QUERY_STRING | sed 's/.*code2=\([^&]*\).*/\1/' )
FILE=$STLPATH/$FILENAME

X1=$( echo $QUERY_STRING | sed 's/.*X1=\([^&]*\).*/\1/' )
Y1=$( echo $QUERY_STRING | sed 's/.*Y1=\([^&]*\).*/\1/' )
X2=$( echo $QUERY_STRING | sed 's/.*X2=\([^&]*\).*/\1/' )
Y2=$( echo $QUERY_STRING | sed 's/.*Y2=\([^&]*\).*/\1/' )


bold=$( echo $QUERY_STRING | sed 's/.*bold=\([^&]*\).*/\1/' )
space=$( echo $QUERY_STRING | sed 's/.*space=\([^&]*\).*/\1/' )
high=$( echo $QUERY_STRING | sed 's/.*high=\([^&]*\).*/\1/' )




if [ ! -f $FILE ]; then
	echo "Content-type: text/html"
	echo
	echo "<p>Error file $FILE not found</p>"
	exit 
fi


############## RUN OPENSCAD ###############


export OPENSCADPATH=/usr/lib/cgi-bin

TMP_OUT=$(tempfile --suffix .stl)
TMP_SCAD=$(tempfile --suffix .scad)
trap "rm -f ${TMP_OUT} ${TMP_SCAD}" EXIT

#trap "rm -f $TMP_SCAD" EXIT

echo "
use <Write.scad>

difference() {
	difference() {
		difference() {
		    import_stl(\"$FILE\", 4);
		    translate([0, 0, -1])
		    import_stl(\"/var/www/stl/logo.stl\", 4);
		}
		translate([$X1, $Y1, 0])
		mirror([1,0,0])
		write(\"$CODE1\",t=2,h=$high,center=true,space=$space,bold=$bold);
	}
	translate([$X2, $Y2, 0])
	mirror([1,0,0])
	write(\"$CODE2\",t=2,h=$high,center=true,space=$space,bold=$bold);
}
" > $TMP_SCAD


openscad $TMP_SCAD -o $TMP_OUT


########### RETURN FILE TO USER ########

OUTNAME=$(basename $FILENAME .stl)_${CODE1}${CODE2}.stl

echo "Content-type: application/sla"
echo "Content-Disposition: inline; filename=\"$OUTNAME\""
echo
cat $TMP_OUT
#cat $TMP_SCAD

#echo "$X $Y $bold $space"

