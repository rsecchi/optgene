#!/usr/bin/python

FREECADPATH='/usr/lib/freecad/lib'  # path to FreeCAD lib

import sys
sys.path.append(FREECADPATH)


import FreeCAD
import Mesh

#Mesh.insert(u"/var/www/html/out_stl/Mambretti_FM003_14RXS.stl","Unnamed")
Mesh.insert(sys.argv[1],"Unnamed")


__objs__=[]
__objs__.append(FreeCAD.getDocument("Unnamed").getObject(sys.argv[3]))
Mesh.export(__objs__,sys.argv[2])

del __objs__


