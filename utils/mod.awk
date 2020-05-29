#!/usr/bin/awk -f

/nan/ {next}

/^\f/ {next}

/^--/ {next}

/^Index/ { a=1; next}

/^$/ { a=0 }

a==1

