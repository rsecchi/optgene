#!/bin/bash

# Test program to evolve a circuit

TMP=$(tempfile)
trap "rm -rf $TMP" EXIT

ngspice -b 2> /dev/null <( 

# ngspice program

echo \
"Common-base BJT amplifier
vsupply 1 0 dc 5
vin 2 0 dc
r1 %3 %3 200
r2 %3 %3 200
r3 %3 %3 200
q1 %3 %3 %3 mod1
q2 %3 %3 %3 mod1
q3 %3 %3 %3 mod1
q4 %3 %3 %3 mod1
q5 %3 %3 %3 mod1
q6 %3 %3 %3 mod1
q7 %3 %3 %3 mod1
q8 %3 %3 %3 mod1
q9 %3 %3 %3 mod1
q10 %3 %3 %3 mod1
q11 %3 %3 %3 mod1
q12 %3 %3 %3 mod1
.model mod1 npn bf=50 
.dc vin 0 5 0.01
.print dc v(3)
.end"

) | awk '
	# Extracting Results
	/nan/ {next}
	/^\f/ {next}
	/^--/ {next}
	/^Index/ { a=1; next}
	/^$/ { a=0 }
	a==1
' > $TMP

if [ ! -s $TMP ]; then
	echo -2000000000
	exit
fi

if [ $(cat $TMP | wc -l) != "501" ]; then
	echo -2000000000
	exit
fi

# Evaluating fitness
echo "	
	a=load('$TMP');
	disp(-sum(abs(a(:,3)-5*(a(:,2)>2.5))))
" | octave -q


