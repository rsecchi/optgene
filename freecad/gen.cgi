#!/bin/bash


#QUERY_STRING="filename=Tazzina.stl&code=ABC&Submit=Submit+Query"


############### PARSE ARGS ################


STLPATH=/var/www/stl

FILENAME=$( echo $QUERY_STRING | sed 's/.*filename=\(.*\)$/\1/' )
DESIGNER=$( echo $QUERY_STRING | sed 's/.*designer=\(.*\)&.*$/\1/' )
#FILENAME="cubotest.stl"
#CODE1=$( echo $QUERY_STRING | sed 's/.*code1=\([^&]*\).*/\1/' )
#CODE2=$( echo $QUERY_STRING | sed 's/.*code2=\([^&]*\).*/\1/' )


CODE1=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 2 | head -n 1)
CODE2=$(cat /dev/urandom | tr -dc 'A-Z0-9' | fold -w 3 | head -n 1)


FILE=$STLPATH/$DESIGNER/$FILENAME
OUTNAME=${DESIGNER}_$(basename $FILENAME .stl)_${CODE1}${CODE2}.stl

#X1=$( echo $QUERY_STRING | sed 's/.*X1=\([^&]*\).*/\1/' )
#Y1=$( echo $QUERY_STRING | sed 's/.*Y1=\([^&]*\).*/\1/' )
#X2=$( echo $QUERY_STRING | sed 's/.*X2=\([^&]*\).*/\1/' )
#Y2=$( echo $QUERY_STRING | sed 's/.*Y2=\([^&]*\).*/\1/' )

X1=-0.75
Y1=3.2
X2=-0.75
Y2=-3.2

bold=1.15
space=1.5
high=4.6

#bold=$( echo $QUERY_STRING | sed 's/.*bold=\([^&]*\).*/\1/' )
#space=$( echo $QUERY_STRING | sed 's/.*space=\([^&]*\).*/\1/' )
#high=$( echo $QUERY_STRING | sed 's/.*high=\([^&]*\).*/\1/' )



if [ ! -f $FILE ]; then
	echo "Content-type: text/html"
	echo
	echo "<p>Error file $FILE not found</p>"
	exit 
fi


############## RUN OPENSCAD ###############

(
START=$(date +%s)

exec 1>&-

export OPENSCADPATH=/usr/lib/cgi-bin

TMP_OUT=$(tempfile --suffix .stl)
TMP_OBJ=$(basename $TMP_OUT .stl)
TMP_BIN_OUT=$(tempfile --prefix bin_ --suffix .stl)
TMP_SCAD=$(tempfile  --suffix .scad)

trap "rm -f ${TMP_OUT} ${TMP_SCAD} " EXIT
#trap "rm -f $TMP_SCAD" EXIT

echo "
use <Write.scad>

difference() {
	difference() {
		import_stl(\"$FILE\", 4);
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

/usr/lib/cgi-bin/exportbin.py $TMP_OUT $TMP_BIN_OUT $TMP_OBJ

mv $TMP_BIN_OUT /var/www/html/out_stl/${OUTNAME}

END=$(date +%s)

{
	echo "<p>${OUTNAME}  "
	echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}';
	echo "</p>"
} >> /var/www/html/out_stl/times.html


) &


#echo $FILENAME >> $TMP_SCAD

########### RETURN PAGE TO USER ########

#
#echo "Content-type: application/sla"
#echo "Content-Disposition: inline; filename=\"$OUTNAME\""
#echo
#cat $TMP_OUT


echo "Content-type: text/html"
echo
echo "<p>Please wait!</p>"
echo "<p>The file name is ${OUTNAME}</p>"
echo "<p>The file will be placed in out_stl</p>"


#echo "Content-Disposition: inline; filename=\"$OUTNAME\""


