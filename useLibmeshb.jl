
include("libMeshb.jl")

using .libMeshb

meshFileName = "examples/naca0012.mesh"
mesh = readMesh(meshFileName)