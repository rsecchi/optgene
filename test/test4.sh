#!/bin/bash

# Test program to evolve a circuit

TMP=$(
ngspice 2> /dev/null -b <( 

# ngspice program

echo \
"*
vs 1 0 dc 5
vin 2 0 dc 0 PULSE(0 5 0 1n 1n 0.5m 1m 100)
vt out_target 0 PULSE(0 5 0 1n 1n 1m 2m 100)
r1 %3 %3 1000
r2 %3 %3 1000
r3 %3 %3 1000
r4 %3 %3 1000
r5 %3 %3 1000
r6 %3 %3 1000
r7 %3 %3 10000
r8 %3 %3 10000
r9 %3 %3 10000
r10 %3 %3 10000
q1 %3 %3 %3 2N3904
q2 %3 %3 %3 2N3904
q3 %3 %3 %3 2N3904
q4 %3 %3 %3 2N3904
q5 %3 %3 %3 2N3904
q6 %3 %3 %3 2N3904
q7 %3 %3 %3 2N3904
q8 %3 %3 %3 2N3904
q9 %3 %3 %3 2N3904
q10 %3 %3 %3 2N3904
q11 %3 %3 %3 2N3904
q12 %3 %3 %3 2N3904
.model 2N3904 NPN IS=1E-14 VAF=100 Bf=300 IKF=0.4 XTB=1.5 BR=4 CJC=4E-12  CJE=8E-12 RB=20 RC=0.1 RE=0.1 TR=250E-9 TF=350E-12 ITF=1 VTF=2 XTF=3 Vceo=40 Icrating=200m
.tran 0.0001 0.005
.meas tran result rms par('V(out_target)-V(3)')
.end"

) | awk '/^result/ {print -$3}' 
)

if [ "$TMP" = "" ]; then
	echo -2000000000
	exit
else
	echo $TMP
fi

