#!/usr/bin/python

import cgitb, sys, cgi

cgitb.enable()

#print('Content-type: text/html\n')

print('Content-type: application/sla')
print('Content-Disposition: inline; filename="Tazzina.stl"\n')

#data = open('Tazzina.stl', 'rb').read()
#sys.stdout.write(data)

args = cgi.FieldStorage()

#for i in arguments.keys():
#	if ( i == "filename" ):
#		filename = arguments[i].value

if 'filename' in args.keys():
	filename = args['filename'].value
	data = open('Tazzina.stl', 'rb').read()
	sys.stdout.write(data)



