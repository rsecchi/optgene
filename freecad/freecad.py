#!/usr/bin/python

def out_error(errstr):
	print "Content-type: text/html\n"
	print "<p>Error: %s</p>" % (errstr)


import cgitb, sys, cgi, os
FREECADPATH='/usr/lib/freecad/lib'  # path to FreeCAD dll
sys.path.append(FREECADPATH)

import FreeCAD, Mesh, Part, BuildRegularGeoms

cgitb.enable()

#print "Content-type: text/html\n"

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

# print "Opening %s" % (myfile)


#if 'filename' in args.keys():
#	filename = args['filename'].value
#	data = open('Tazzina.stl', 'rb').read()
#	sys.stdout.write(data)


sys.stdout.flush()
newstdout = os.dup(1)
devnull = os.open(os.devnull, os.O_WRONLY)
os.dup2(devnull, 1)
os.close(devnull)
sys.stdout = os.fdopen(newstdout, 'w')


d = FreeCAD.newDocument()

# Loading requested file
mesh1 = Mesh.Mesh(myfile)
d.addObject("Mesh::Feature", "mwork")
d.mwork.Mesh = mesh1

# Creating a cylinder
cyl = BuildRegularGeoms.Cylinder(2.0, 10.0, True, 1.0, 50)
cyl_mesh = Mesh.Mesh(cyl)
d.addObject("Mesh::Feature", "mcyl")
d.mcyl.Mesh = cyl_mesh

# Merge the two solids
umesh = d.mwork.Mesh.unite(d.mcyl.Mesh)
d.addObject("Mesh::Feature", "Union")
d.Union.Mesh = umesh
d.recompute()

# Write on a file
d.getObject("Union").Mesh.write("union2.stl", "STL", "Union")

#print('Content-type: application/sla')
#print('Content-Disposition: inline; filename="Tazzina.stl"\n')

#data = open('Tazzina.stl', 'rb').read()
#sys.stdout.write(data)

