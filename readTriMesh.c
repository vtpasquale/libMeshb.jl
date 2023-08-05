#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "libMeshb/libmeshb7.h"


int readMeshInfo(char *meshFileName, int *ver, int *dim, int *nNodes,int *nTriangles,int *nEdges)
{
    int64_t LibIdx;

    // Open the mesh file for reading
    LibIdx = GmfOpenMesh(meshFileName, GmfRead, &ver, &dim );

    // Get the numbers of entities
    *nNodes = GmfStatKwd( LibIdx, GmfVertices  );
    *nTriangles = GmfStatKwd( LibIdx, GmfTriangles );
    *nEdges = GmfStatKwd( LibIdx, GmfEdges );

    // Close the mesh file !
    GmfCloseMesh( LibIdx ); 

    return 0;
}

int readMesh(char *meshFileName,int nNodes,int nTriangles,int nEdges,double (*nodes)[4],int (*triangles)[4],int (*edges)[3])
{
    int i;

    int64_t LibIdx;
    int verTemp, dimTemp;

    // Open the mesh file for reading
    LibIdx = GmfOpenMesh(meshFileName, GmfRead, &verTemp, &dimTemp );

    // Move the file pointer to the vertices keyword
    GmfGotoKwd( LibIdx, GmfVertices );

    // Read nodes
    for(i=0;i<nNodes;i++)
        GmfGetLin( LibIdx, GmfVertices, &nodes[i][0], &nodes[i][1], &nodes[i][2], &nodes[i][3] );

    // Move the file pointer to the triangles keyword
    GmfGotoKwd( LibIdx, GmfTriangles );

    // Read triangles
    for(i=0;i<nTriangles;i++)
        GmfGetLin( LibIdx, GmfTriangles, &triangles[i][0], &triangles[i][1], &triangles[i][2], &triangles[i][3] );


    // Move the file pointer to the edges keyword
    GmfGotoKwd( LibIdx, GmfEdges );

    // Read edges
    for(i=0;i<nEdges;i++)
        GmfGetLin( LibIdx, GmfEdges, &edges[i][0], &edges[i][1], &edges[i][2] );

    // Close the mesh file !
    GmfCloseMesh( LibIdx );

    return 0;
}

// Main used for testing methods in C before interfacing with Julia.
// The memory for shared variables will be allocated by Julia and 
// modifed by this C library. 
int main(int argc, char *argv[])
{
    int64_t LibIdx;
    int i, ver, dim, nNodes, nTriangles, nEdges, (*triangles)[4], (*edges)[3];
    double (*nodes)[4];

    readMeshInfo(argv[1], &ver, &dim, &nNodes, &nTriangles, &nEdges);

    printf("This is file version: %d\n",ver);

    // Allocate memory
    nodes     = malloc( nNodes     * 4 * sizeof(double) );
    triangles = malloc( nTriangles * 4 * sizeof(int)   );
    edges     = malloc( nEdges     * 3 * sizeof(int)   );

    readMesh(argv[1], nNodes, nTriangles, nEdges, nodes, triangles, edges);
    
    printf("\nFirst Nodes\n");
    for(i=0;i<3;i++)
        printf("%f,%f,%f,%f\n",nodes[i][0],nodes[i][1],nodes[i][2],nodes[i][3]);

    printf("\nLast Nodes\n");
    for(int i=nNodes-3;i<nNodes;i++)
        printf("%f,%f,%f,%f\n",nodes[i][0],nodes[i][1],nodes[i][2],nodes[i][3]);

    printf("\nFirst Triangles\n");
    for(i=0;i<3;i++)
        printf("%d,%d,%d,%d\n",triangles[i][0],triangles[i][1],triangles[i][2],triangles[i][3]);
    
    printf("\nLast Triangles\n");
    for(i=nTriangles-3;i<nTriangles;i++)
        printf("%d,%d,%d,%d\n",triangles[i][0],triangles[i][1],triangles[i][2],triangles[i][3]);

    printf("\nFirst Edges\n");
    for(i=0;i<3;i++)
        printf("%d,%d,%d\n",edges[i][0],edges[i][1],edges[i][2]);
    
    printf("\nLast Edges\n");
    for(i=nEdges-3;i<nEdges;i++)
        printf("%d,%d,%d\n",edges[i][0],edges[i][1],edges[i][2]);        

    return 0;
    
}