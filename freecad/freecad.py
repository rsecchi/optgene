#!/usr/bin/python

def out_error(errstr):
	print "Content-type: text/html\n"
	print "<p>Error: %s</p>" % (errstr)


import cgitb, sys, cgi
FREECADPATH='/usr/lib/freecad/lib'  # path to FreeCAD dll
sys.path.append(FREECADPATH)
import FreeCAD, Mesh, Part

cgitb.enable()

print "Content-type: text/html\n"

#print('Content-type: application/sla')
#print('Content-Disposition: inline; filename="Tazzina.stl"\n')

#data = open('Tazzina.stl', 'rb').read()
#sys.stdout.write(data)

args = cgi.FieldStorage()


# Look for "filename" amongst the arguments
myfile="nofile"
for i in args.keys():
	if ( i == "filename" ):
		myfile=args[i].value
	#	filename = arguments[i].value


# Try to open myfile
if (myfile == "nofile"):
	out_error("filename not provided")
	sys.exit(1)
else:
	try:
		stlfile = open(myfile,'rb')
	except IOError:
		out_error("Cannot open file")
		sys.exit(1)

print "Opening %s" % (myfile)


#if 'filename' in args.keys():
#	filename = args['filename'].value
#	data = open('Tazzina.stl', 'rb').read()
#	sys.stdout.write(data)



