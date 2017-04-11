#!/bin/bash


# Detach from the web server (closing stdout)
exec 1>&-


# Only one instance at a time
#FILELOCK=/tmp/wedo.lock
#lockfile -r 1 ${FILELOCK}

START=$(date +%s)



export OPENSCADPATH=/usr/lib/cgi-bin


############ CREATE TEMPFILES ##################### 

TMP_OUT=$(tempfile --suffix .stl)
TMP_OBJ=$(basename $TMP_OUT .stl)
TMP_BIN_OUT=$(tempfile --prefix bin_ --suffix .stl)
TMP_SCAD=$(tempfile  --suffix .scad)
TMP_REM=$(tempfile --suffix .stl)


trap "rm -f ${TMP_OUT} ${TMP_SCAD} /tmp/wedo.lock ${TMP_BIN_OUT} ${TMP_REM}" EXIT


############ SPLIT SOLID   ####################

/usr/local/bin/stl_split  $FILE 2> /tmp/cize
admesh /tmp/stlout_solid.stl -b /tmp/stlout_fixed.stl

############ RUN OPENSCAD  ##################### 

echo "
use <Write.scad>

difference() {
	difference() {
		difference() {
			import_stl(\"/tmp/stlout_fixed.stl\", 4);
			translate([$X1, $Y1, 0])
			mirror([1,0,0])
			write(\"$CODE1\",t=2,h=$high,center=true,space=$space,bold=$bold);
		}
		translate([$X2, $Y2, 0])
		mirror([1,0,0])
		write(\"$CODE2\",t=2,h=$high,center=true,space=$space,bold=$bold);
	}
	translate([0, 0, -0.5])
	import_stl(\"/var/www/stl/logotest.stl\", 4);
}
" > $TMP_SCAD

openscad $TMP_SCAD -o $TMP_OUT 
$CGIPATH/exportbin.py $TMP_OUT $TMP_BIN_OUT $TMP_OBJ

/usr/local/bin/stl_remove $TMP_BIN_OUT $TMP_REM
/usr/local/bin/stl_join $TMP_REM /tmp/stlout_remain.stl /tmp/ooo.stl >> /tmp/cize

cp $TMP_REM /tmp/lid.stl

############## UPDATE OUTPOUT ###################


admesh /tmp/ooo.stl -b /var/www/html/test_stl/${OUTNAME}


END=$(date +%s)
{
	echo "<p>${OUTNAME}  "
	echo $((END-START)) | awk '{print int($1/60)":"int($1%60)}';
	echo "</p>"
} >> /var/www/html/test_stl/time.html



