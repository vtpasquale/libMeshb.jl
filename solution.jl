
# Get entity sizes so solution data array can be preallocated
function readSolutionInfo(solutionFileName::AbstractString)

    # Create references controled by Julia to be modifed by C 
    versionRef = Ref{Cint}(0)
    dimensionRef = Ref{Cint}(0)
    nLinesRef = Ref{Cint}(0)
    nResultsRef = Ref{Cint}(0)

    # Call the C libaray
    rv = @ccall "./readTriMesh.so".readSolutionInfo(solutionFileName::Cstring,versionRef::Ref{Cint},dimensionRef::Ref{Cint},nLinesRef::Ref{Cint},nResultsRef::Ref{Cint})::Cint

    # convert to numbers from zero-dimensional arrays
    version=versionRef[]
    dimension=dimensionRef[]
    nLines=nLinesRef[]
    nResults=nResultsRef[]

    return (version,dimension,nLines,nResults)
end    

function readSolutionData(solutionFileName,nLines,nResults)

    solution = Array{Cdouble,1}(undef,nLines*nResults)  
    solutionRef = pointer(solution)

    rv = @ccall "./readTriMesh.so".readSolutionData(solutionFileName::Cstring,solutionRef::Ptr{Array{Cdouble,2}})::Cint

    # Seperate results into seperate columns
    solutionOut = Array{Cdouble,2}(undef,nLines,nResults)
    for r in 1:nResults
        solutionOut[:,r] = solution[r:nResults:end]
    end

    return solutionOut

end

function readSolution(solutionFileName::AbstractString)

    # Check that solution file exists
    if !isfile(solutionFileName)
        println("""Solution file "$solutionFileName" not found.""")
        return 1
    end
    
    # Read solution information
    (version,dimension,nLines,nResults) = readSolutionInfo(solutionFileName)

    # Read and return solution data
    return readSolutionData(solutionFileName,nLines,nResults)

end