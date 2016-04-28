#!/usr/bin/python

def out_error(errstr):
	print "Content-type: text/html\n"
	print "<p>Error: %s</p>" % (errstr)


import cgitb, sys, cgi
FREECADPATH='/usr/lib/freecad/lib'  # path to FreeCAD dll
sys.path.append(FREECADPATH)

import FreeCAD, Mesh, Part, BuildRegularGeoms

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



#sys.stdout.flush()
#newstdout = os.dup(1)
#devnull = os.open(os.devnull, os.O_WRONLY)
#os.dup2(devnull, 1)
#os.close(devnull)
#sys.stdout = os.fdopen(newstdout, 'w')



# Loading requested file
mesh1 = Mesh.Mesh(myfile)

# Creating a cylinder
cyl = BuildRegularGeoms.Cylinder(2.0, 10.0, True, 1.0, 50)
cyl_mesh = Mesh.Mesh(cyl)
cyl2 = BuildRegularGeoms.Cylinder(3.0, 5.0, True, 1.0, 50)
cyl_mesh2 = Mesh.Mesh(cyl2)
cyl_mesh.unite(cyl_mesh2)

# mesh2 = Mesh.Mesh("cubo2.stl")
# Mesh.insert("cubo2.stl","Unnamed")

mesh1.unite(cyl_mesh)

print "ma vai a cagarei..."

#mesh = m1.unite(m2)

#mesh = App.ActiveDocument.Tazzina.Mesh.unite(App.ActiveDocument.cubo2.Mesh)
#App.activeDocument().addObject("Mesh::Feature","Union")
#App.activeDocument().Union.Mesh = mesh

#App.ActiveDocument.recompute()
#FreeCAD.ActiveDocument.getObject("Union").Mesh.write("union2.stl","STL","Union"


f = open('workfile.stl','w')
f.close()

cyl_mesh2.write("workfile.stl")




