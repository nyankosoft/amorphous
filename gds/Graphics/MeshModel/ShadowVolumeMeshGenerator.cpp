#include "ShadowVolumeMeshGenerator.hpp"
#include "Graphics/MeshModel/3DMeshModelArchive.hpp"
#include "Graphics/TextureUtilities.hpp"


namespace amorphous
{

using namespace std;


/// Takes an array of CEdgeMapping objects, then returns an index for the edge in the
/// table if such entry exists, or returns an index at which a new entry for the edge
/// can be written.
/// nV1 and nV2 are the vertex indexes for the old edge.
/// nCount is the number of elements in the array.
/// The function returns -1 if an available entry cannot be found.  In reality,
/// this should never happens as we should have allocated enough memory.
int CShadowVolumeMeshGenerator::FindEdgeInMappingTable( int nV1, int nV2, CEdgeMapping *pMapping, int nCount )
{
    for( int i = 0; i < nCount; ++i )
    {
        // If both vertex indexes of the old edge in mapping entry are -1, then
        // we have searched every valid entry without finding a match.  Return
        // this index as a newly created entry.
        if( ( pMapping[i].m_anOldEdge[0] == -1 && pMapping[i].m_anOldEdge[1] == -1 ) ||

            // Or if we find a match, return the index.
            ( pMapping[i].m_anOldEdge[1] == nV1 && pMapping[i].m_anOldEdge[0] == nV2 ) )
        {
            return i;
        }
    }

    return -1;  // We should never reach this line
}


/// Takes a mesh and generate a new mesh from it that contains the degenerate invisible
/// quads for shadow volume extrusion.
bool CShadowVolumeMeshGenerator::GenerateShadowMesh( C3DMeshModelArchive& rShadowVolumeMesh,
													 C3DMeshModelArchive& rOriginalMesh )
{

	// Generate adjacency information


	bool bResult;

	C3DMeshModelArchive sv_mesh;
	C3DMeshModelArchive patch_mesh;

	vector<unsigned short> vecusPtReps;

	rOriginalMesh.GeneratePointRepresentatives( vecusPtReps );

	CMMA_VertexSet& rOrigVertex = rOriginalMesh.GetVertexSet();
	vector<unsigned int>& rOrigIndex = rOriginalMesh.GetVertexIndex();

	int iNumOrigVertices = rOrigVertex.GetNumVertices();
	int iNumOrigFaces    = (int)rOrigIndex.size() / 3;

	// Maximum number of unique edges = Number of faces * 3
	DWORD dwNumEdges = iNumOrigFaces * 3;
	CEdgeMapping *pMapping = new CEdgeMapping[dwNumEdges];

	if( !pMapping )
		return false;

	int nNumMaps = 0;  // Number of entries that exist in pMapping

	int iNumSVMeshFaces =    iNumOrigFaces + dwNumEdges * 2;
	int iNumSVMeshVertices = iNumOrigFaces * 3;

	// Create a new mesh
/*	ID3DXMesh *pNewMesh;
	hr = D3DXCreateMesh( &pNewMesh );*/

	// nNextIndex is the array index in IB that the next vertex index value
	// will be store at.
	int nNextIndex = 0;

	CMMA_VertexSet& rSVMeshVertex = sv_mesh.GetVertexSet();
	vector<unsigned int>& rSVMeshIndex = sv_mesh.GetVertexIndex();

	rSVMeshVertex.vecPosition.resize( iNumSVMeshVertices );
	rSVMeshVertex.vecNormal.resize( iNumSVMeshVertices );

	if( rOrigVertex.GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
	{
		rSVMeshVertex.vecfMatrixWeight.resize( iNumSVMeshVertices );
		rSVMeshVertex.veciMatrixIndex.resize( iNumSVMeshVertices );
		
	}

	rSVMeshIndex.resize( iNumSVMeshFaces * 3, 0 );

	// pNextOutVertex is the location to write the next
	// vertex to.
	int iNextOutVertex = 0;

//					SHADOWVERTEX *pNextOutVertex = pNewVBData;



	// Iterate through the faces.  For each face, output new
	// vertices and face in the new mesh, and write its edges
	// to the mapping table.

	for( int f = 0; f < iNumOrigFaces; ++f )
	{
//		CopyMemory();

		// Copy the vertex data for all 3 vertices
		int j;
		for( j=0; j<3; j++ )
		{
			int orig_index = rOrigIndex[f * 3 + j];
			int next_index = iNextOutVertex + j;

			rSVMeshVertex.vecPosition[ next_index ]	= rOrigVertex.vecPosition[ orig_index ];
			rSVMeshVertex.vecNormal[ next_index ]	= rOrigVertex.vecNormal[ orig_index ];

			if( rOrigVertex.GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
			{
				rSVMeshVertex.vecfMatrixWeight[ next_index ] = rOrigVertex.vecfMatrixWeight[ orig_index ];
				rSVMeshVertex.veciMatrixIndex[ next_index ]  = rOrigVertex.veciMatrixIndex[ orig_index ];
			}
		}


		// Write out the face
		rSVMeshIndex[nNextIndex++] = f * 3;
		rSVMeshIndex[nNextIndex++] = f * 3 + 1;
		rSVMeshIndex[nNextIndex++] = f * 3 + 2;

		// Compute the face normal and assign it to
		// the normals of the vertices.
		Vector3 v1, v2;  // v1 and v2 are the edge vectors of the face
		Vector3 vNormal;
//		v1 = *(Vector3*)(pNextOutVertex + 1) - *(Vector3*)pNextOutVertex;
//		v2 = *(Vector3*)(pNextOutVertex + 2) - *(Vector3*)(pNextOutVertex + 1);
		v1 = rSVMeshVertex.vecPosition[iNextOutVertex + 1] - rSVMeshVertex.vecPosition[iNextOutVertex];
		v2 = rSVMeshVertex.vecPosition[iNextOutVertex + 2] - rSVMeshVertex.vecPosition[iNextOutVertex + 1];
		Vec3Cross( vNormal, v1, v2 );
		Vec3Normalize( vNormal, vNormal );

/*		pNextOutVertex->Normal = (pNextOutVertex + 1)->Normal = (pNextOutVertex + 2)->Normal = vNormal;*/

		rSVMeshVertex.vecNormal[iNextOutVertex]   = vNormal;
		rSVMeshVertex.vecNormal[iNextOutVertex+1] = vNormal;
		rSVMeshVertex.vecNormal[iNextOutVertex+2] = vNormal;

		iNextOutVertex += 3;
//		pNextOutVertex += 3;

		// Add the face's edges to the edge mapping table

		// Edge 1
		int nIndex;
		int nVertIndex[3]
		    = { vecusPtReps[rOrigIndex[f * 3]],
		        vecusPtReps[rOrigIndex[f * 3 + 1]],
		        vecusPtReps[rOrigIndex[f * 3 + 2]] };



		nIndex = FindEdgeInMappingTable( nVertIndex[0], nVertIndex[1], pMapping, dwNumEdges );

		// If error, we are not able to proceed, so abort.
		if( -1 == nIndex )
		{
			bResult = false;
			Cleanup( sv_mesh, patch_mesh );
//			goto cleanup;
		}

		if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
		{
			// No entry for this edge yet.  Initialize one.
			pMapping[nIndex].m_anOldEdge[0] = nVertIndex[0];
			pMapping[nIndex].m_anOldEdge[1] = nVertIndex[1];
			pMapping[nIndex].m_aanNewEdge[0][0] = f * 3;
			pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 1;

			++nNumMaps;
		}
		else
		{
			// An entry is found for this edge.  Create
			// a quad and output it.
			assert( nNumMaps > 0 );

			pMapping[nIndex].m_aanNewEdge[1][0] = f * 3;	  // For clarity
			pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 1;

			// First triangle
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

			// Second triangle
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

			// pMapping[nIndex] is no longer needed. Copy the last map entry
			// over and decrement the map count.

			pMapping[nIndex] = pMapping[nNumMaps-1];
			FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
			--nNumMaps;
		}

		// Edge 2
		nIndex = FindEdgeInMappingTable( nVertIndex[1], nVertIndex[2], pMapping, dwNumEdges );

		// If error, we are not able to proceed, so abort.
		if( -1 == nIndex )
		{
			bResult = false;
			Cleanup( sv_mesh, patch_mesh );
//			goto cleanup;
		}

		if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
		{
			pMapping[nIndex].m_anOldEdge[0] = nVertIndex[1];
			pMapping[nIndex].m_anOldEdge[1] = nVertIndex[2];
			pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 1;
			pMapping[nIndex].m_aanNewEdge[0][1] = f * 3 + 2;

			++nNumMaps;
		}
		else
		{
			// An entry is found for this edge.  Create
			// a quad and output it.
			assert( nNumMaps > 0 );

			pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 1;
			pMapping[nIndex].m_aanNewEdge[1][1] = f * 3 + 2;

			// First triangle
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

			// Second triangle
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

			// pMapping[nIndex] is no longer needed. Copy the last map entry
			// over and decrement the map count.

			pMapping[nIndex] = pMapping[nNumMaps-1];
			FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
			--nNumMaps;
		}

		// Edge 3
		nIndex = FindEdgeInMappingTable( nVertIndex[2], nVertIndex[0], pMapping, dwNumEdges );

		// If error, we are not able to proceed, so abort.
		if( -1 == nIndex )
		{
			bResult = false;
			Cleanup( sv_mesh, patch_mesh );
//			goto cleanup;
		}

		if( pMapping[nIndex].m_anOldEdge[0] == -1 && pMapping[nIndex].m_anOldEdge[1] == -1 )
		{
			pMapping[nIndex].m_anOldEdge[0] = nVertIndex[2];
			pMapping[nIndex].m_anOldEdge[1] = nVertIndex[0];
			pMapping[nIndex].m_aanNewEdge[0][0] = f * 3 + 2;
			pMapping[nIndex].m_aanNewEdge[0][1] = f * 3;

			++nNumMaps;
		}
		else
		{
			// An entry is found for this edge.  Create
			// a quad and output it.
			assert( nNumMaps > 0 );

			pMapping[nIndex].m_aanNewEdge[1][0] = f * 3 + 2;
			pMapping[nIndex].m_aanNewEdge[1][1] = f * 3;

			// First triangle
/*			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];*/
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][1];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];

			// Second triangle
/*			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			pdwNewIBData[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];*/
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][1];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[1][0];
			rSVMeshIndex[nNextIndex++] = pMapping[nIndex].m_aanNewEdge[0][0];

			// pMapping[nIndex] is no longer needed. Copy the last map entry
			// over and decrement the map count.

			pMapping[nIndex] = pMapping[nNumMaps-1];
			FillMemory( &pMapping[nNumMaps-1], sizeof( pMapping[nNumMaps-1] ), 0xFF );
			--nNumMaps;
		}
	}


	if( 0 == nNumMaps )
	{
		SetShadowVolumeMeshProperties( sv_mesh, rOriginalMesh );
		rShadowVolumeMesh = sv_mesh;
//		delete [] pdwPtRep;
		delete [] pMapping;
		return true;
	}

	// Now the entries in the edge mapping table represent
	// non-shared edges.  What they mean is that the original
	// mesh has openings (holes), so we attempt to patch them.
	// First we need to recreate our mesh with a larger vertex
	// and index buffers so the patching geometry could fit.

//	DXUTTRACE( L"Faces to patch: %d\n", nNumMaps );

	// Create a mesh with large enough vertex and
	// index buffers.

/*	SHADOWVERTEX *pPatchVBData = NULL;
	DWORD *pdwPatchIBData = NULL;

	ID3DXMesh *pPatchMesh = NULL;
	// Make enough room in IB for the face and up to 3 quads for each patching face
	hr = D3DXCreateMesh( &pPatchMesh );

	if( pPatchVBData && pdwPatchIBData )
	{
		ZeroMemory();
		CopyMemory();// Copy the data from one mesh to the other
	}
	else
	{
		// Some serious error is preventing us from locking.
		// Abort and return error.

		pPatchMesh->Release();
		goto cleanup;
	}

	// Replace pNewMesh with the updated one.  Then the code
	// can continue working with the pNewMesh pointer.

	pNewVBData = pPatchVBData;
	pdwNewIBData = pdwPatchIBData;
	pNewMesh->Release();
	pNewMesh = pPatchMesh;
*/


//	C3DMeshModelArchive patch_mesh;
	CMMA_VertexSet& rPMVertex = patch_mesh.GetVertexSet();
	vector<unsigned int>& rPMIndex = patch_mesh.GetVertexIndex();

	rPMVertex.vecPosition.resize( ( iNumOrigFaces + nNumMaps ) * 3 );
	rPMVertex.vecNormal.resize( ( iNumOrigFaces + nNumMaps ) * 3 );

	rPMIndex.resize( ( nNextIndex / 3 + nNumMaps * 7 ) * 3 );

    if( rOrigVertex.GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
	{
		rPMVertex.vecfMatrixWeight.resize( ( iNumOrigFaces + nNumMaps ) * 3 );
		rPMVertex.veciMatrixIndex.resize( ( iNumOrigFaces + nNumMaps ) * 3 );
	}

	int vert;
	for( vert=0; vert<iNumOrigFaces * 3; vert++ )
	{
		rPMVertex.vecPosition[vert] = rSVMeshVertex.vecPosition[vert];
		rPMVertex.vecNormal[vert]   = rSVMeshVertex.vecNormal[vert];

		if( rOrigVertex.GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
		{
			rPMVertex.vecfMatrixWeight[ vert ] = rSVMeshVertex.vecfMatrixWeight[ vert ];
			rPMVertex.veciMatrixIndex[ vert ]  = rSVMeshVertex.veciMatrixIndex[ vert ];
		}
	}

	int index;
	for( index=0; index<nNextIndex; index++ )
	{
		rPMIndex[index] = rSVMeshIndex[index];
	}

	// Now, we iterate through the edge mapping table and
	// for each shared edge, we generate a quad.
	// For each non-shared edge, we patch the opening
	// with new faces.

	// nNextVertex is the index of the next vertex.
	int nNextVertex = iNumOrigFaces * 3;

	for( int i = 0; i < nNumMaps; ++i )
	{
		if( pMapping[i].m_anOldEdge[0] != -1 &&
			pMapping[i].m_anOldEdge[1] != -1 )
		{
			// If the 2nd new edge indexes is -1,
			// this edge is a non-shared one.
			// We patch the opening by creating new
			// faces.
			if( pMapping[i].m_aanNewEdge[1][0] == -1 ||  // must have only one new edge
				pMapping[i].m_aanNewEdge[1][1] == -1 )
			{
				// Find another non-shared edge that
				// shares a vertex with the current edge.
				for( int i2 = i + 1; i2 < nNumMaps; ++i2 )
				{

					if( pMapping[i2].m_anOldEdge[0] != -1 &&	   // must have a valid old edge
						pMapping[i2].m_anOldEdge[1] != -1 &&
						( pMapping[i2].m_aanNewEdge[1][0] == -1 || // must have only one new edge
						pMapping[i2].m_aanNewEdge[1][1] == -1 ) )
					{
						int nVertShared = 0;
						if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
							++nVertShared;
						if( pMapping[i2].m_anOldEdge[1] == pMapping[i].m_anOldEdge[0] )
							++nVertShared;

						if( 2 == nVertShared )
						{
							// These are the last two edges of this particular
							// opening. Mark this edge as shared so that a degenerate
							// quad can be created for it.

							pMapping[i2].m_aanNewEdge[1][0] = pMapping[i].m_aanNewEdge[0][0];
							pMapping[i2].m_aanNewEdge[1][1] = pMapping[i].m_aanNewEdge[0][1];
							break;
						}
						else
						if( 1 == nVertShared )
						{
							// nBefore and nAfter tell us which edge comes before the other.
							int nBefore, nAfter;
							if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
							{
								nBefore = i;
								nAfter = i2;
							}
							else
							{
								nBefore = i2;
								nAfter = i;
							}

							// Found such an edge. Now create a face along with two
							// degenerate quads from these two edges.

/*							pNewVBData[nNextVertex]   = pNewVBData[pMapping[nAfter].m_aanNewEdge[0][1]];
							pNewVBData[nNextVertex+1] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][1]];
							pNewVBData[nNextVertex+2] = pNewVBData[pMapping[nBefore].m_aanNewEdge[0][0]];*/
							rPMVertex.vecPosition[nNextVertex]   = rPMVertex.vecPosition[pMapping[nAfter].m_aanNewEdge[0][1]];
							rPMVertex.vecPosition[nNextVertex+1] = rPMVertex.vecPosition[pMapping[nBefore].m_aanNewEdge[0][1]];
							rPMVertex.vecPosition[nNextVertex+2] = rPMVertex.vecPosition[pMapping[nBefore].m_aanNewEdge[0][0]];

							if( rOrigVertex.GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
							{
								rPMVertex.vecfMatrixWeight[nNextVertex]   = rPMVertex.vecfMatrixWeight[pMapping[nAfter].m_aanNewEdge[0][1]];
								rPMVertex.vecfMatrixWeight[nNextVertex+1] = rPMVertex.vecfMatrixWeight[pMapping[nBefore].m_aanNewEdge[0][1]];
								rPMVertex.vecfMatrixWeight[nNextVertex+2] = rPMVertex.vecfMatrixWeight[pMapping[nBefore].m_aanNewEdge[0][0]];
								rPMVertex.veciMatrixIndex[nNextVertex]   = rPMVertex.veciMatrixIndex[pMapping[nAfter].m_aanNewEdge[0][1]];
								rPMVertex.veciMatrixIndex[nNextVertex+1] = rPMVertex.veciMatrixIndex[pMapping[nBefore].m_aanNewEdge[0][1]];
								rPMVertex.veciMatrixIndex[nNextVertex+2] = rPMVertex.veciMatrixIndex[pMapping[nBefore].m_aanNewEdge[0][0]];
							}

							// Recompute the normal
//							Vector3 v1 = pNewVBData[nNextVertex+1].Position - pNewVBData[nNextVertex].Position;
//							Vector3 v2 = pNewVBData[nNextVertex+2].Position - pNewVBData[nNextVertex+1].Position;
							Vector3 v1 = rPMVertex.vecPosition[nNextVertex+1] - rPMVertex.vecPosition[nNextVertex];
							Vector3 v2 = rPMVertex.vecPosition[nNextVertex+2] - rPMVertex.vecPosition[nNextVertex+1];
							Vec3Normalize( v1, v1 );
							Vec3Normalize( v2, v2 );
							Vec3Cross( rPMVertex.vecNormal[nNextVertex], v1, v2 );
							rPMVertex.vecNormal[nNextVertex+1] = rPMVertex.vecNormal[nNextVertex+2] = rPMVertex.vecNormal[nNextVertex];

/*							pdwNewIBData[nNextIndex] = nNextVertex;
							pdwNewIBData[nNextIndex+1] = nNextVertex + 1;
							pdwNewIBData[nNextIndex+2] = nNextVertex + 2;*/
							rPMIndex[nNextIndex] = nNextVertex;
							rPMIndex[nNextIndex+1] = nNextVertex + 1;
							rPMIndex[nNextIndex+2] = nNextVertex + 2;

							// 1st quad

							rPMIndex[nNextIndex+3] = pMapping[nBefore].m_aanNewEdge[0][1];
							rPMIndex[nNextIndex+4] = pMapping[nBefore].m_aanNewEdge[0][0];
							rPMIndex[nNextIndex+5] = nNextVertex + 1;

							rPMIndex[nNextIndex+6] = nNextVertex + 2;
							rPMIndex[nNextIndex+7] = nNextVertex + 1;
							rPMIndex[nNextIndex+8] = pMapping[nBefore].m_aanNewEdge[0][0];

							// 2nd quad

							rPMIndex[nNextIndex+9] = pMapping[nAfter].m_aanNewEdge[0][1];
							rPMIndex[nNextIndex+10] = pMapping[nAfter].m_aanNewEdge[0][0];
							rPMIndex[nNextIndex+11] = nNextVertex;

							rPMIndex[nNextIndex+12] = nNextVertex + 1;
							rPMIndex[nNextIndex+13] = nNextVertex;
							rPMIndex[nNextIndex+14] = pMapping[nAfter].m_aanNewEdge[0][0];

							// Modify mapping entry i2 to reflect the third edge
							// of the newly added face.

							if( pMapping[i2].m_anOldEdge[0] == pMapping[i].m_anOldEdge[1] )
							{
								pMapping[i2].m_anOldEdge[0] = pMapping[i].m_anOldEdge[0];
							}
							else
							{
								pMapping[i2].m_anOldEdge[1] = pMapping[i].m_anOldEdge[1];
							}
							pMapping[i2].m_aanNewEdge[0][0] = nNextVertex + 2;
							pMapping[i2].m_aanNewEdge[0][1] = nNextVertex;

							// Update next vertex/index positions

							nNextVertex += 3;
							nNextIndex += 15;

							break;
						}
					}
				}
			}
			else
			{
				// This is a shared edge.  Create the degenerate quad.

				// First triangle
				rPMIndex[nNextIndex++] = pMapping[i].m_aanNewEdge[0][1];
				rPMIndex[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
				rPMIndex[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];

				// Second triangle
				rPMIndex[nNextIndex++] = pMapping[i].m_aanNewEdge[1][1];
				rPMIndex[nNextIndex++] = pMapping[i].m_aanNewEdge[1][0];
				rPMIndex[nNextIndex++] = pMapping[i].m_aanNewEdge[0][0];
			}
		}
	}

	SetShadowVolumeMeshProperties( patch_mesh, rOriginalMesh );
	rShadowVolumeMesh = patch_mesh;
//	delete [] pdwPtReps;
	delete [] pMapping;
	return true;

//cleanup:;

	Cleanup( sv_mesh, patch_mesh );


	return false;

/*				if( SUCCEEDED( hr ) )
			{
				// At this time, the output mesh may have an index buffer
				// bigger than what is actually needed, so we create yet
				// another mesh with the exact IB size that we need and
				// output it.  This mesh also uses 16-bit index if
				// 32-bit is not necessary.

				DXUTTRACE( L"Shadow volume has %u vertices, %u faces.\n", ( iNumOrigFaces + nNumMaps ) * 3, nNextIndex / 3 );

				bool bNeed32Bit = ( iNumOrigFaces + nNumMaps ) * 3 > 65535;
				ID3DXMesh *pFinalMesh;
				hr = D3DXCreateMesh( &pFinalMesh );
				if( SUCCEEDED( hr ) )
				{
					if( pNewVBData && pdwNewIBData && pFinalVBData && pwFinalIBData )
					{
						CopyMemory();
						if( bNeed32Bit ) CopyMemory();
						else{ for( int i = 0; i < nNextIndex; ++i )	pwFinalIBData[i] = (WORD)pdwNewIBData[i]; }
					}*/

//	return hr;
}


void CShadowVolumeMeshGenerator::Cleanup( C3DMeshModelArchive& rSVMesh, C3DMeshModelArchive& rPatchMesh )
{
	rSVMesh.GetVertexSet().Clear();
	rSVMesh.GetVertexIndex().clear();

	rPatchMesh.GetVertexSet().Clear();
	rPatchMesh.GetVertexIndex().clear();
}


void CShadowVolumeMeshGenerator::SetShadowVolumeMeshProperties( C3DMeshModelArchive& mesh,
															    C3DMeshModelArchive& rSrcMesh )
{

	// vertex format - position & normal
	mesh.GetVertexSet().m_VertexFormatFlag = CMMA_VertexSet::VF_POSITION|CMMA_VertexSet::VF_NORMAL;

	// triangle set - set a single triangle set that covers the entire mesh
	CMMA_TriangleSet triangle_set;
	triangle_set.m_iMinIndex = 0;
	triangle_set.m_iStartIndex = 0;
	triangle_set.m_iNumTriangles = (int)mesh.GetVertexIndex().size() / 3;
	triangle_set.m_iNumVertexBlocksToCover = mesh.GetVertexSet().GetNumVertices();

	mesh.GetTriangleSet().clear();
	mesh.GetTriangleSet().push_back( triangle_set );

	// material - default textures
	CMMA_Material material;
	material.fSpecular = 0;
	material.vecTexture.resize( 2 );
//	material.vecTexture[0].type = CMMA_Texture::ARCHIVE_32BITCOLOR;
//	material.vecTexture[0].vecTexelData.resize( 1, 1, S32BitColor(255,255,255,255) );
//	material.vecTexture[1].type = CMMA_Texture::ARCHIVE_32BITCOLOR;
//	material.vecTexture[1].vecTexelData.resize( 1, 1, S32BitColor(128,128,255,255) );
	SetSingleColorTextureDesc( material.vecTexture[0], SFloatRGBAColor(1.0f,1.0f,1.0f,1.0f) );
	SetSingleColorTextureDesc( material.vecTexture[1], SFloatRGBAColor(0.5f,0.5f,1.0f,1.0f) );

	if( rSrcMesh.GetVertexSet().GetVertexFormat() & CMMA_VertexSet::VF_WEIGHT )
	{
		mesh.GetVertexSet().RaiseVertexFormatFlag( CMMA_VertexSet::VF_WEIGHT );

		// copy vertex blend weights
//		mesh.GetVertexSet().vecfMatrixWeight = rSrcMesh.GetVertexSet().vecfMatrixWeight;
//		mesh.GetVertexSet().veciMatrixIndex= rSrcMesh.GetVertexSet().veciMatrixIndex;
	}

	// copy the skeleton structure of the original mesh
	if( 1 < rSrcMesh.GetNumBones() )
	{
		mesh.CopySkeletonFrom( rSrcMesh );
	}

//	material.SurfaceTexture.type = CMMA_Texture::FILENAME;
//	material.SurfaceTexture.strFilename = "Default.bmp";
//	material.NormalMapTexture.type = CMMA_Texture::FILENAME;
//	material.NormalMapTexture.strFilename = "Default_NM.bmp";
	mesh.GetMaterial().clear();
	mesh.GetMaterial().push_back( material );
}

/*

 xyzzy00    Member since: 11/23/2001  From: Irvine, CA, United States 
 
 Posted - 5/29/2004 1:54:19 PM 
Yes, it is an array of vertex indices. 

If you think of two adjacent triangles which share an edge. Lets say triangle ABC and DEF share an edge BC/DE like:

---CD---
--/||\--
-A-||-F-
--\||/--
---BE--- (the '-'s are just for spacing)

Vertices: ABCDEF
Adjacency: -1-0--

Vertices B and E are co-located (since we know the edge are shared), but may have different vertex indices, since they might have different normals, texture coordinates, colors, etc.. Vertices C and D are co-located too.

When you convert to point-reps, a single vertex index will be used to represent these co-located vertex. For example, if you created point reps for the triangles above, it would probably look like:

Vertices: ABCDEF
Point-reps: 012215

Note that B and E are both given the value 1. The fact that they share a value indicates that they are co-located. Since 1 is the index of B, B is called the point-representative of B and E.

The same is true of C and D. In this example, C is the point-representative of C and D. It is not really important that C was picked instead of D to be the representative. Either would work just as well.

Hope this all makes some sense.

xyzzy
 
*/



} // namespace amorphous
