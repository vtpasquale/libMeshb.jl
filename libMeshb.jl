
module libMeshb

export Mesh, readMesh

struct Mesh
    version::Integer
    dimension::Integer
    nodes::Array{AbstractFloat,2}
    triagnles::Array{Integer,2}
    edges::Array{Integer,2}
end

# Get entity sizes so mesh data arrays can be preallocated
function readMeshInfo(meshFileName::AbstractString)

    # Create references controled by Julia to be modifed by C 
    versionRef = Ref{Cint}(0)
    dimensionRef = Ref{Cint}(0)
    nNodesRef = Ref{Cint}(0)
    nTrianglesRef = Ref{Cint}(0)
    nEdgesRef = Ref{Cint}(0)

    # Call the C libaray
    rv = @ccall "./readTriMesh.so".readMeshInfo(meshFileName::Cstring,versionRef::Ref{Cint},dimensionRef::Ref{Cint},nNodesRef::Ref{Cint},nTrianglesRef::Ref{Cint},nEdgesRef::Ref{Cint})::Cint

    # convert to numbers from zero-dimensional arrays
    version=versionRef[]
    dimension=dimensionRef[]
    nNodes=nNodesRef[]
    nTriangles=nTrianglesRef[]
    nEdges=nEdgesRef[]

    return (version,dimension,nNodes,nTriangles,nEdges)
end

# Get entity sizes so mesh data arrays can be preallocated
function readMeshData(meshFileName::AbstractString,nNodes::Integer,nTriangles::Integer,nEdges::Integer)

    # Create arrays controled by Julia to be modifed by C 
    nodes = Array{Cdouble,2}(undef,4,nNodes)
    triangles = Array{Cint,2}(undef,4,nTriangles)
    edges = Array{Cint,2}(undef,3,nEdges)

    # Create pointers to Arrays. References didn't work for me.
    nodesRef = pointer(nodes)
    trianglesRef = pointer(triangles)
    edgesRef = pointer(edges)
    # nodesRef = Ref(Node)
    # trianglesRef = Ref(Triangle)

    # Call the C libaray
    rv = @ccall "./readTriMesh.so".readMesh(meshFileName::Cstring,nNodes::Cint,nTriangles::Cint,nEdges::Cint,nodesRef::Ptr{Array{Cdouble,2}},trianglesRef::Ptr{Array{Cint,2}},edgesRef::Ptr{Array{Cint,2}})::Cint
    # rv = @ccall "./readTriMesh.so".getMesh(meshFileName::Cstring,nNodes::Cint,nTriangles::Cint,nodesRef::Ref{Array{Cdouble,2}},trianglesRef::Ref{Array{Cint,2}})::Cint

    # Data needed to be transpose of C data to work.
    nodesOut = transpose(nodes)
    trianglesOut = transpose(triangles)
    edgesOut = transpose(edges)

    return (nodesOut,trianglesOut,edgesOut)
end

function readMesh(meshFileName::AbstractString)

    # Check that mesh file exists
    if !isfile(meshFileName)
        println("""Mesh file "$meshFileName" not found.""")
        return 1
    end
    
    # Read mesh information
    (version,dimension,nNodes,nTriangles,nEdges) = readMeshInfo(meshFileName)

    # Read mesh data
    (nodes,triangles,edges) = readMeshData(meshFileName,nNodes,nTriangles,nEdges);

    return Mesh(version,dimension,nodes,triangles,edges)

end

end

# meshFileName = "examples/naca0012.mesh"
# mesh = readMesh(meshFileName)