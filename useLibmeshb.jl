
include("libMeshb.jl")

using .libMeshb

# Read mesh
meshFileName = "examples/naca0012.meshb"
mesh = readMesh(meshFileName)

#Read solution
solutionFileName = "examples/naca0012_v3.solb"
solution = readSolution(solutionFileName)

# Compute own solution = node locations
nNodes = size(mesh.nodes,1)
mySolution = Array{Float64,2}(undef,nNodes,4)
mySolution[:,1] = mesh.nodes[:,1]
mySolution[:,2] = mesh.nodes[:,1].^2
mySolution[:,3] = mesh.nodes[:,1].^3
mySolution[:,4] = mesh.nodes[:,1].^4

# Write own solution
nLines = size(mySolution,1)
nResults = size(mySolution,2)
solution = Array{Float64}(undef,nLines*nResults)
for i in 1:nResults
    solution[i:nResults:nResults*nLines-(nResults-i)] = mySolution[:,i]
end
solutionRef = pointer(solution)

version = 3
dimension = 3
solutionFileName = "examples/naca0012_wrt.solb"
rv = @ccall "./readTriMesh.so".writeSolutionData(solutionFileName::Cstring,version::Cint,dimension::Cint,nLines::Cint,nResults::Cint,solutionRef::Ptr{Array{Cdouble}})::Cint
