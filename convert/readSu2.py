# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import numpy as np
import meshio

m = meshio.read("square.su2")

nodes     = m.points
triangles = m.get_cells_type("triangle") + 1
edges     = m.get_cells_type("line") + 1
edgeIDs   = m.get_cell_data('su2:tag','line')

nNodes = np.size(nodes,0);
nTriangles = np.size(triangles,0)
nEdges = np.size(edges,0)

with open("square.mesh","w") as fid:
    print("MeshVersionFormatted 3",file=fid)
    print("",file=fid)
    print("Dimension 2",file=fid)
    print("",file=fid)
    print("Vertices",file=fid)
    print(nNodes,file=fid)
    for i in range(nNodes):
        print(f"{nodes[i,0]} {nodes[i,1]} 1",file=fid)
    
    print("",file=fid)
    print("Edges",file=fid)
    print(nEdges,file=fid)
    for i in range(nEdges):
        print(f"{edges[i,0]} {edges[i,1]} {edgeIDs[i]}",file=fid)
        
    print("",file=fid)
    print("Triangles",file=fid)
    print(nTriangles,file=fid)
    for i in range(nTriangles):
        print(f"{triangles[i,0]} {triangles[i,1]} {triangles[i,2]} 1",file=fid)
    
    print("",file=fid)
    print("End",file=fid)