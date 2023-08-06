
module libMeshb

export Mesh, readMesh, readSolution, writeSolution

struct Mesh
    version::Integer
    dimension::Integer
    nodes::Array{AbstractFloat,2}
    triangles::Array{Integer,2}
    edges::Array{Integer,2}
end

include("mesh.jl")
include("solution.jl")

end