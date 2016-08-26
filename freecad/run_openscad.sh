#!/bin/bash


# Only one instance at a time
FILELOCK=/tmp/wedo.lock
lockfile -r 1 ${FILELOCK}

START=$(date +%s)


# Detach from the web server (closing stdout)
exec 1>&-

export OPENSCADPATH=/usr/lib/cgi-bin


############ CREATE TEMPFILES ##################### 

TMP_OUT=$(tempfile --suffix .stl)
TMP_OBJ=$(basename $TMP_OUT .stl)
TMP_BIN_OUT=$(tempfile --prefix bin_ --suffix .stl)
TMP_SCAD=$(tempfile  --suffix .scad)

trap "rm -f ${TMP_OUT} ${TMP_SCAD} ${FILELOCK} ${TMP_BIN_OUT}" EXIT



############ RUN OPENSCAD  ##################### 

echo "
use <Write.scad>

difference() {
	difference() {
		import_stl(\"$FILE\", 4);
		translate([-0.75, 3.2, 0])
		mirror([1,0,0])
		write(\"$CODE1\",t=2,h=4.6,center=true,space=1.5,bold=1.15);
	}
	translate([-0.75, -3.2, 0])
	mirror([1,0,0])
	write(\"$CODE2\",t=2,h=4.6,center=true,space=1.5,bold=1.15);
}
" > $TMP_SCAD

openscad $TMP_SCAD -o $TMP_OUT 
$CGIPATH/exportbin.py $TMP_OUT $TMP_BIN_OUT $TMP_OBJ


############## UPDATE OUTPOUT ###################


mv $TMP_BIN_OUT /var/www/html/out_stl/${OUTNAME}


END=$(date +%s)
{
	echo "<p>${OUTNAME}  "
	echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}';
	echo "</p>"
} >> /var/www/html/out_stl/times.html



