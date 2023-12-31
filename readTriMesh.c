#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "libMeshb/libmeshb7.h"


int readMeshInfo(char *meshFileName, int *ver, int *dim, int *nNodes,int *nTriangles,int *nEdges)
{
    int64_t LibIdx;

    // Open the mesh file for reading
    LibIdx = GmfOpenMesh(meshFileName, GmfRead, ver, dim );

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

int readSolutionInfo(char *meshFileName, int *thisVer, int *thisDim, int *NbrLin, int *SolSiz)
{
    int64_t idx;
    int NbrTyp, TypTab[ GmfMaxTyp ];

    /* Try to open the file and ensure its version is 3
    (double precision reals) and dimension is 2 */
    idx = GmfOpenMesh(meshFileName, GmfRead, thisVer, thisDim);
    if (!idx)
    {
        printf("Cannot open mesh/solution file %s\n", meshFileName);
        return 1;
    }
    *NbrLin = GmfStatKwd(idx, GmfSolAtVertices, &NbrTyp, SolSiz, TypTab);
    printf("NbrLin = %d, NmbTyp = %d, SolSiz = %d\n", *NbrLin, NbrTyp, *SolSiz);

    // SolTab = malloc( NbrLin*SolSiz * sizeof(double)   );

    // // Solution field block reading
    // GmfGetBlock(idx, GmfSolAtVertices, 1, NbrLin, 0, NULL, NULL,
    //             GmfDoubleVec, SolSiz, &SolTab[0], &SolTab[(NbrLin*SolSiz)-1]);

    // Print each solutions of each vertices
    // for (i = 0; i < NbrLin; i++)
    // {
    //     printf("%f %f %f %f\n", SolTab[i * SolSiz + 0], SolTab[i * SolSiz + 1], SolTab[i * SolSiz + 2], SolTab[i * SolSiz + 3]);
    // }

    // Close the mesh file
    GmfCloseMesh(idx);

    return 0;
}

int readSolutionData(char *meshFileName, double (*SolTab)[1])
{
    int i;

    int64_t idx;
    int verTemp, dimTemp, NbrLin, SolSiz, NbrTyp, TypTab[ GmfMaxTyp ];

    // Open the mesh file for reading
    idx = GmfOpenMesh(meshFileName, GmfRead, &verTemp, &dimTemp );
    NbrLin = GmfStatKwd(idx, GmfSolAtVertices, &NbrTyp, &SolSiz, TypTab);

    // // Print data
    // printf("meshFileName: %s\n",meshFileName);
    // printf("ver: %d\n",verTemp);
    // printf("NbrTyp: %d\n",NbrTyp);
    // printf("SolSiz: %d\n",SolSiz);
    // for(int i=0;i<10;i++)
    //     printf("  TypTab[]=%d\n",TypTab[i]);

    // Solution field block reading
    GmfGetBlock(idx, GmfSolAtVertices, 1, NbrLin, 0, NULL, NULL,
                GmfDoubleVec, SolSiz, SolTab[0], SolTab[(NbrLin*SolSiz)-1]);

    // Close the mesh file !
    GmfCloseMesh( idx );

    return 0;
}

int writeSolutionData(char *outFileName, int ver, int dim, int NbrLin, int SolSiz, double (*SolTab)[1])
{
    int64_t OutMsh;
    OutMsh = GmfOpenMesh(outFileName, GmfWrite, ver, dim);
    if (!OutMsh)
    {
        printf("Issue accessing %s for writing output\n", outFileName);
        return 1;
    }

    // Hardcode solution type to all scalar doubles
    int NbrTyp = SolSiz;
    int TypTab[ GmfMaxTyp ];
    for(int i=0;i<SolSiz;i++)
        TypTab[i]=1;

    // Write data
    GmfSetKwd(OutMsh, GmfSolAtVertices, NbrLin, NbrTyp, &TypTab);  

    GmfSetBlock(OutMsh, GmfSolAtVertices, 1, NbrLin, 0, NULL, NULL,
         GmfDoubleVec, SolSiz, &SolTab[0], &SolTab[(NbrLin*SolSiz)-1]);

    GmfCloseMesh(OutMsh);

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
    
    // int NbrLin = nNodes;
    // int SolSiz = 1;
    // double (*SolTab)[1];
    // SolTab = malloc( nNodes * sizeof(double)   );
    // for(i=0;i<nNodes;i++)
    //     *SolTab[i] = nodes[i][0];
    // writeSolutionData("testout.sol", ver, dim, NbrLin, SolSiz, SolTab);

    // printf("\nFirst Nodes\n");
    // for(i=0;i<3;i++)
    //     printf("%f,%f,%f,%f\n",nodes[i][0],nodes[i][1],nodes[i][2],nodes[i][3]);

    // printf("\nLast Nodes\n");
    // for(int i=nNodes-3;i<nNodes;i++)
    //     printf("%f,%f,%f,%f\n",nodes[i][0],nodes[i][1],nodes[i][2],nodes[i][3]);

    // printf("\nFirst Triangles\n");
    // for(i=0;i<3;i++)
    //     printf("%d,%d,%d,%d\n",triangles[i][0],triangles[i][1],triangles[i][2],triangles[i][3]);
    
    // printf("\nLast Triangles\n");
    // for(i=nTriangles-3;i<nTriangles;i++)
    //     printf("%d,%d,%d,%d\n",triangles[i][0],triangles[i][1],triangles[i][2],triangles[i][3]);

    // printf("\nFirst Edges\n");
    // for(i=0;i<3;i++)
    //     printf("%d,%d,%d\n",edges[i][0],edges[i][1],edges[i][2]);
    
    // printf("\nLast Edges\n");
    // for(i=nEdges-3;i<nEdges;i++)
    //     printf("%d,%d,%d\n",edges[i][0],edges[i][1],edges[i][2]);        

    return 0;
    
}