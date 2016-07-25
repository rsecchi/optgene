#!/usr/bin/python

FREECADPATH='/usr/lib/freecad/lib'  # path to FreeCAD dll
STLPATH='/var/www/stl/'
FONTFILE='/home/raffaello/Desktop/Ubuntu-B.ttf'

import cgitb, sys, cgi, os, math
sys.stdout.flush()
newstdout = os.dup(1)
devnull = os.open(os.devnull, os.O_WRONLY)
os.dup2(devnull, 1)
os.close(devnull)
sys.stdout = os.fdopen(newstdout, 'w')

def out_error(errstr):
	print "Content-type: text/html\n"
	print "<p>Error: %s</p>" % (errstr)


def code_to_text(textstr, doc, tmpf):
	ss = Draft.makeShapeString(textstr, FONTFILE, 3, 0)
	doc.addObject("Part::Extrusion", "Extrude")
	doc.Extrude.Base = doc.ShapeString
	doc.Extrude.Dir = (0, 0, 2)
	doc.Extrude.Solid = (True)
	doc.Extrude.TaperAngle = (0)
	doc.recompute()
	pos = FreeCAD.Vector(-10.0,-2.0,0.0)
	rot = App.Rotation(0,0,0)
	cen = FreeCAD.Vector(0.0,0.0,0.0)
	#pos = FreeCAD.Placement(p,r)
	d.getObject("Extrude").Placement = App.Placement(pos, rot, cen)
	objs = []
	objs.append(d.getObject("Extrude"))
	Mesh.export(objs, tmpf)

sys.path.append(FREECADPATH)

import FreeCAD 
import Mesh, Part, BuildRegularGeoms, Draft
import tempfile

#cgitb.enable()
args = cgi.FieldStorage()


# Look for "filename" among the arguments
infile="nofile"
for i in args.keys():
	if ( i == "filename" ):
		infile=args[i].value
	#	filename = arguments[i].value



myfile = STLPATH + infile

# Try to open myfile
if (infile == "nofile"):
	out_error("filename not provided")
	sys.exit(1)
else:
	try:
		stlfile = open(myfile,'rb')
	except IOError:
		out_error("Cannot open file")
		sys.exit(1)

# print "Opening %s" % (myfile)

# Look for "code" in args
mycode="nocode"
for i in args.keys():
	if ( i == "code" ):
		mycode=args[i].value


if (mycode=="nocode" or mycode==""):
	out_error("Code not provided")
	sys.exit(1)


# print myfile
# tt = os.path.splitext(myfile)
# bname = tt[0] + '_' + mycode + tt[1]
# print bname

#if 'filename' in args.keys():
#	filename = args['filename'].value
#	data = open('Tazzina.stl', 'rb').read()
#	sys.stdout.write(data)


d = FreeCAD.newDocument()

# Loading requested file
mesh1 = Mesh.Mesh(myfile)
d.addObject("Mesh::Feature", "mwork")
d.mwork.Mesh = mesh1


def create_disc():
	# Creating a cylinder
	cyl = BuildRegularGeoms.Cylinder(10.0, 2.0, True, 1.0, 50)
	cyl_mesh = Mesh.Mesh(cyl)
	d.addObject("Mesh::Feature", "mcyl")
	d.mcyl.Mesh = cyl_mesh
	#d.getObject("Union").Placement = 
	pos = FreeCAD.Vector(0.0,0.0,0.0)
	rot = App.Rotation(0,90,0)
	cen = FreeCAD.Vector(0.0,0.0,0.0)
	#pos = FreeCAD.Placement(p,r)
	d.getObject("mcyl").Placement = App.Placement(pos, rot, cen)
	#d.recompute()


#tf = tempfile.mktemp() + ".stl"

tf = '/tmp/letters.stl'
code_to_text(mycode, d, tf)


def load_text(tf):
	mtext = Mesh.Mesh(tf)
	d.addObject("Mesh::Feature", "mtxt")
	return mtext

mtext = load_text(tf)
#mtext = d.mcyl.Mesh

# Merge the two solids
umesh = d.mwork.Mesh.difference(mtext)
d.addObject("Mesh::Feature", "Union")
d.Union.Mesh = umesh
d.recompute()


# Return the merge on stdout
def output_file_and_exit():
	fname = '"' + os.path.splitext(infile)[0] + '_' + mycode + '.stl"'
	cd_header = 'Content-Disposition: inline; filename='+fname+'\n'
	print('Content-type: application/sla')
	print(cd_header)
	tmpfilename = tempfile.mktemp()
	d.getObject("Union").Mesh.write(tmpfilename, "STL", "Union")
	f = open(tmpfilename, 'r+b')
	ff = f.read()
	sys.stdout.write(ff)

# Return the merge in a file
def output_workfile():
	d.getObject("Union").Mesh.write("workfile.stl", "STL", "Union")


output_workfile()

#data = open('Tazzina.stl', 'rb').read()
#sys.stdout.write(data)

#out_error("Operation Completed")
#output_file_and_exit()

