#include "OBBTree.hpp"
#include "3DMath/Matrix34.hpp"
#include "Support/Log/DefaultLog.hpp"
#include <float.h>


namespace amorphous
{


OBBTree::OBBTree()
{
	pOBBTopNode = new OBBNODE;
}

OBBTree::~OBBTree()
{
	ReleaseNodes( pOBBTopNode );
}

void OBBTree::ReleaseNodes( OBBNODE *pOBBNode )
{
	if( pOBBNode->pOBBNodeL ) ReleaseNodes( pOBBNode->pOBBNodeL );
	if( pOBBNode->pOBBNodeR ) ReleaseNodes( pOBBNode->pOBBNodeR );

	delete pOBBNode;
	pOBBNode = NULL;
}
/*
bool OBBTree::Create( LPD3DXMESH pMesh, int Level )
{
	// Get vertices
	int VertexNum = pMesh->GetNumVertices();
	std::vector < Vector3 > Vertex;
	LPDIRECT3DVERTEXBUFFER9 pVB;
	pMesh->GetVertexBuffer( &pVB );

	D3DVERTEXBUFFER_DESC VDesc;
	pVB->GetDesc( &VDesc );
	int FVFSize = VDesc.Size / VertexNum;
	float *pf;
	pVB->Lock( 0, 0, (void**)&pf, D3DLOCK_DISCARD );
	for( int i = 0; i < VertexNum; ++i ){
		Vector3 Vec;
		Vec.x = *(pf++);
		Vec.y = *(pf++);
		Vec.z = *(pf++);
		Vertex.push_back( Vec );

		// Do the per-byte operations.
		// You can access the vertices as 4-byte data, though,
		// because the vertex format can only be either 'float' or 'DWORD'.
		pf = (float *)( (char *)pf + ( FVFSize - 4 * 3 ) );
	}
	pVB->Unlock();

	// Get triangle polygons
	int FaceNum = pMesh->GetNumFaces();
	FACES Faces;
	LPDIRECT3DINDEXBUFFER9 pIB;
	pMesh->GetIndexBuffer( &pIB );

	D3DINDEXBUFFER_DESC IDesc;
	pIB->GetDesc( &IDesc );

	WORD  *pw = NULL;
	DWORD *pd = NULL;
	if( IDesc.Format == D3DFMT_INDEX16 ) pIB->Lock( 0, 0, (void**)&pw, D3DLOCK_DISCARD );
	else if( IDesc.Format == D3DFMT_INDEX32 ) pIB->Lock( 0, 0, (void**)&pd, D3DLOCK_DISCARD );
	else return false;

	for( int i = 0; i < FaceNum; ++i ){
		FACE Face;
		if( IDesc.Format == D3DFMT_INDEX16 ){
			Face.Vertex[0] = Vertex[*(pw++)];
			Face.Vertex[1] = Vertex[*(pw++)];
			Face.Vertex[2] = Vertex[*(pw++)];
		}
		else if( IDesc.Format == D3DFMT_INDEX32 ){
			Face.Vertex[0] = Vertex[*(pd++)];
			Face.Vertex[1] = Vertex[*(pd++)];
			Face.Vertex[2] = Vertex[*(pd++)];
		}
		Face.Center = ( Face.Vertex[0] + Face.Vertex[1] + Face.Vertex[2] ) / 3;
		Faces.push_back( Face );
	}
	pIB->Unlock();

	Create( Faces, Level, pOBBTopNode );
	
	return true;
}
*/
bool OBBTree::Create( const std::vector<Vector3>& vertices, const std::vector<unsigned int>& triangle_indices, int Level )
{
	FACES Faces;

	if( triangle_indices.size() % 3 != 0 )
	{
		LOG_PRINT_WARNING( fmt_string( " The number of the indices, %d, is not a multiple of three. Specify the indices of the triangulated polygons.", triangle_indices.size() ) );
		return false;
	}

	const unsigned int num_triangles = (unsigned int)triangle_indices.size() / 3;
	Faces.resize( num_triangles );
	for( unsigned int i=0; i<num_triangles; i++ )
	{
		Faces[i].Vertex[0] = vertices[ triangle_indices[i*3  ] ];
		Faces[i].Vertex[1] = vertices[ triangle_indices[i*3+1] ];
		Faces[i].Vertex[2] = vertices[ triangle_indices[i*3+2] ];
		Faces[i].Center    = ( Faces[i].Vertex[0] + Faces[i].Vertex[1] + Faces[i].Vertex[2] ) / 3;
	}

	Create( Faces, Level, pOBBTopNode );

	return true;
}

void OBBTree::Create( FACES &Faces, int Level, OBBNODE *pOBBNode )
{
	CreateOBB( Faces, pOBBNode->OBBData );

	// Sort the triangles depending on whether they are right of left of the center on the longest axis.
	// Try another axis if they cannot be divided with the longest axis.
	FACES FacesL, FacesR;
	for( int j = 0; j < 3; ++j ){
		FacesL.clear();
		FacesR.clear();
		for( int i = 0; i < (int)Faces.size(); ++i ){
			Vector3 Vec = pOBBNode->OBBData.Matrix * Faces[i].Center;
			float v[3] = { Vec.x, Vec.y, Vec.z };

			if( v[j] < pOBBNode->OBBData.Length[j] / 2 ) FacesL.push_back( Faces[i] );
			else FacesR.push_back( Faces[i] );
		}
		if( FacesL.size() > 0 && FacesR.size() > 0 ) break;
	}

	--Level;
	if( Level < 0 ) return;

	if( FacesL.size() > 0 ){
		pOBBNode->pOBBNodeL = new OBBNODE;
		Create( FacesL, Level, pOBBNode->pOBBNodeL );
	}
	if( FacesR.size() > 0 ){
		pOBBNode->pOBBNodeR = new OBBNODE;
		Create( FacesR, Level, pOBBNode->pOBBNodeR );
	}
}

// Create an OBB which contains the given polygons
void OBBTree::CreateOBB( std::vector < FACE > &Face, OBBDATA &OBBData )
{
	std::vector < Vector3 > Vertex;
	for( int j = 0; j < (int)Face.size(); ++j ){
		for( int i = 0; i < 3; ++i ){
			Vertex.push_back( Face[j].Vertex[i] );
		}
	}

	int Size = (int)Vertex.size();
	// average
	Vector3 m( 0, 0, 0 );
	for( int i = 0; i < Size; ++i ){
		m += Vertex[i];
	}
	m /= (float)Size;

	// Calculate the covariance matrix
	float C11 = 0, C22 = 0, C33 = 0, C12 = 0, C13 = 0,  C23 = 0;
	for( int i = 0; i < Size; ++i ){
		C11 += ( Vertex[i].x - m.x ) * ( Vertex[i].x - m.x );
		C22 += ( Vertex[i].y - m.y ) * ( Vertex[i].y - m.y );
		C33 += ( Vertex[i].z - m.z ) * ( Vertex[i].z - m.z );
		C12 += ( Vertex[i].x - m.x ) * ( Vertex[i].y - m.y );
		C13 += ( Vertex[i].x - m.x ) * ( Vertex[i].z - m.z );
		C23 += ( Vertex[i].y - m.y ) * ( Vertex[i].z - m.z );
	}
	C11 /= Size;
	C22 /= Size;
	C33 /= Size;
	C12 /= Size;
	C13 /= Size;
	C23 /= Size;

	float Matrix[3][3] = {
		{ C11, C12, C13 },
		{ C12, C22, C23 },
		{ C13, C23, C33 },
	};

	// Calculate eigen value & eigen vector via the Jacobi method
	float EigenVectors[3][3];
	float EigenValue[3];
	Jacobi( Matrix, EigenVectors, EigenValue );

	// Sort the eigen value in descending order
	struct SORT{
		int ID;
		float Value;
	} Sort[3] = { { 0, EigenValue[0] }, { 1, EigenValue[1] }, { 2, EigenValue[2] } };

	for( int j = 0; j < 2; ++j ){
		for( int i = 2; i > j; --i ){
			if( Sort[i - 1].Value < Sort[i].Value ){
				SORT a = Sort[i];
				Sort[i] = Sort[i - 1];
				Sort[i - 1] = a;
			}
		}
	}

	for( int i = 0; i < 3; ++i ){
		OBBData.Axis[i].x = EigenVectors[0][Sort[i].ID];
		OBBData.Axis[i].y = EigenVectors[1][Sort[i].ID];
		OBBData.Axis[i].z = EigenVectors[2][Sort[i].ID];
	}

	// Calculate the bounding box
	float min[3] = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
	float max[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	for( int j = 0; j < 3; ++j ){
		for( int i = 0; i < Size; ++i ){
			float a = Vec3Dot( Vertex[i], OBBData.Axis[j] );
			if( min[j] > a ) min[j] = a;
			if( max[j] < a ) max[j] = a;
		}
	}

	OBBData.Center = OBBData.Axis[0] * ( ( min[0] + max[0] ) / 2 )
				   + OBBData.Axis[1] * ( ( min[1] + max[1] ) / 2 )
				   + OBBData.Axis[2] * ( ( min[2] + max[2] ) / 2 );

	for( int i = 0; i < 3; ++i ) OBBData.Length[i] = max[i] - min[i];
	Vector3 G = OBBData.Center
		- (	OBBData.Axis[0] * OBBData.Length[0]
		  + OBBData.Axis[1] * OBBData.Length[1]
		  + OBBData.Axis[2] * OBBData.Length[2] ) / 2;

	OBBData.Matrix.matOrient.SetColumn( 0, Vector3( OBBData.Axis[0].x, OBBData.Axis[1].x, OBBData.Axis[2].x ) );
	OBBData.Matrix.matOrient.SetColumn( 1, Vector3( OBBData.Axis[0].y, OBBData.Axis[1].y, OBBData.Axis[2].y ) );
	OBBData.Matrix.matOrient.SetColumn( 2, Vector3( OBBData.Axis[0].z, OBBData.Axis[1].z, OBBData.Axis[2].z ) );
	OBBData.Matrix.vPosition.x = -Vec3Dot( OBBData.Axis[0], G );
	OBBData.Matrix.vPosition.y = -Vec3Dot( OBBData.Axis[1], G );
	OBBData.Matrix.vPosition.z = -Vec3Dot( OBBData.Axis[2], G );
}

bool OBBTree::IsCollision( OBBDATA &BoxA, OBBDATA &BoxB )
{
	// Transform to the space where 3 axes of BoxA become xyz axes
	Vector3 Axis[3];
	Matrix34 Matrix = BoxA.Matrix;
	for( int i = 0; i < 3; ++i )
		Axis[i] = Matrix.matOrient * BoxB.Axis[i];

	Vector3 BoxA_Center_to_BoxB_Center = BoxB.Center - BoxA.Center;
	Vector3 VecAtoBTrans = Matrix * BoxA_Center_to_BoxB_Center;

	// Do some preprocessings to avoid redundant calculations
	float B[3][3] = {
		{ Axis[0].x, Axis[0].y, Axis[0].z },
		{ Axis[1].x, Axis[1].y, Axis[1].z },
		{ Axis[2].x, Axis[2].y, Axis[2].z }
	};

	float absB[3][3] = {
		{ fabs( B[0][0] ), fabs( B[0][1] ), fabs( B[0][2] ) },
		{ fabs( B[1][0] ), fabs( B[1][1] ), fabs( B[1][2] ) },
		{ fabs( B[2][0] ), fabs( B[2][1] ), fabs( B[2][2] ) }
	};

	float T[3] = { VecAtoBTrans.x, VecAtoBTrans.y, VecAtoBTrans.z }; 

	float a[3] = { BoxA.Length[0] / 2, BoxA.Length[1] / 2, BoxA.Length[2] / 2 };
	float b[3] = { BoxB.Length[0] / 2, BoxB.Length[1] / 2, BoxB.Length[2] / 2 };

	// Run the collision check routines.

	// Separation axes A.Axis[?]
	for( int i = 0; i < 3; ++i ){
		if( fabs( T[i] )
			> a[i] + b[0] * absB[0][i] + b[1] * absB[1][i] + b[2] * absB[2][i] ) return false;
	}

	// Separation axes B.Axis[?]
	for( int i = 0; i < 3; ++i ){
		if( fabs( T[0] * B[i][0] + T[1] * B[i][1] + T[2] * B[i][2] )
			> b[i] + a[0] * absB[i][0] + a[1] * absB[i][1] + a[2] * absB[i][2] ) return false;
	}

	// Separation axes A.Axis[?] x B.Axis[?]
	for( int j0 = 0; j0 < 3; ++j0 ){
		int j1 = ( j0 + 1 ) % 3;
		int j2 = ( j0 + 2 ) % 3;
		for( int i0 = 0; i0 < 3; ++i0 ){
			int i1 = ( i0 + 1 ) % 3;
			int i2 = ( i0 + 2 ) % 3;
			if( fabs( -T[j1] * B[i0][j2] + T[j2] * B[i0][j1] ) 
				> a[j1] * absB[i0][j2] + a[j2] * absB[i0][j1]
				+ b[i1] * absB[i2][j0] + b[i2] * absB[i1][j0] ) return false;
		}
	}

	return true;
}

bool OBBTree::CheckCollision( OBBNODE *pNodeA, OBBNODE *pNodeB, Matrix34 &TransMat, Matrix33 &RotMat )
{
	// Transform the BoxB to the space viewed from BoxA
	OBBDATA OBBDataB = pNodeB->OBBData;

	OBBDataB.Center = TransMat * OBBDataB.Center;
	for( int i = 0; i < 3; ++i )
		OBBDataB.Axis[i] = RotMat * OBBDataB.Axis[i];

	OBBNODE *pAL = pNodeA->pOBBNodeL, *pAR = pNodeA->pOBBNodeR, *pBL = pNodeB->pOBBNodeL, *pBR = pNodeB->pOBBNodeR;
	bool Result = false;
	if( IsCollision( pNodeA->OBBData, OBBDataB ) ){
		if( !pAL && !pAR && !pBL && !pBR ) Result = true;
		else{
			if(  pAL &&  pBL ) if( Result = CheckCollision( pAL, pBL, TransMat, RotMat ) ) goto EXIT;
			if(  pAL &&  pBR ) if( Result = CheckCollision( pAL, pBR, TransMat, RotMat ) ) goto EXIT;
			if(  pAR &&  pBL ) if( Result = CheckCollision( pAR, pBL, TransMat, RotMat ) ) goto EXIT;
			if(  pAR &&  pBR ) if( Result = CheckCollision( pAR, pBR, TransMat, RotMat ) ) goto EXIT;

			if( !pAL &&  pBL ) if( Result = CheckCollision( pNodeA, pBL, TransMat, RotMat ) ) goto EXIT;
			if( !pAL &&  pBR ) if( Result = CheckCollision( pNodeA, pBR, TransMat, RotMat ) ) goto EXIT;
			// Skip these because they are equivalent with the above two.
		//	if( !pAR &&  pBL ) if( Result = CheckCollision( pNodeA, pBL, TransMat, RotMat ) ) goto EXIT;
		//	if( !pAR &&  pBR ) if( Result = CheckCollision( pNodeA, pBR, TransMat, RotMat ) ) goto EXIT;

			if(  pAL && !pBL ) if( Result = CheckCollision( pAL, pNodeB, TransMat, RotMat ) ) goto EXIT;
			if(  pAR && !pBL ) if( Result = CheckCollision( pAR, pNodeB, TransMat, RotMat ) ) goto EXIT;
			// Skip these because they are equivalent with the above two.
		//	if(  pAL && !pBR ) if( Result = CheckCollision( pAL, pNodeB, TransMat, RotMat ) ) goto EXIT;
		//	if(  pAR && !pBR ) if( Result = CheckCollision( pAR, pNodeB, TransMat, RotMat ) ) goto EXIT;
		}
	}
EXIT:
	return Result;
}

bool OBBTree::CheckCollision( OBBTree &OBBTreeA, Matrix34 &TransMatA,
					 OBBTree &OBBTreeB, Matrix34 &TransMatB )
{
	Matrix34 TransMat = TransMatA.GetInverseROT() * TransMatB;
	return CheckCollision( OBBTreeA.pOBBTopNode, OBBTreeB.pOBBTopNode, TransMat, TransMat.matOrient );
}

void OBBTree::GetLeafOBBs( const OBBNODE *pNode, std::vector<OBBDATA>& obbs )
{
	if( pNode->pOBBNodeL == NULL
	 || pNode->pOBBNodeR == NULL )
	{
		obbs.push_back( pNode->OBBData );
	}

	if( pNode->pOBBNodeL ) GetLeafOBBs( pNode->pOBBNodeL, obbs );
	if( pNode->pOBBNodeR ) GetLeafOBBs( pNode->pOBBNodeR, obbs );
}

static inline void indent( FILE *fp, int num_spaces ) { for( int i=0; i<num_spaces; i++ ) fprintf( fp, " " ); }

void OBBTree::DumpToTextFile( FILE *fp, OBBNODE *pNode, int depth )
{
	const OBBDATA& obb = pNode->OBBData;

	indent(fp,depth*2); fprintf( fp, "center %.3f %.3f %.3f\n", obb.Center.x, obb.Center.y, obb.Center.z );
	indent(fp,depth*2); fprintf( fp, "radii %.3f %.3f %.3f\n",  obb.Length[0]*0.5f, obb.Length[1]*0.5f, obb.Length[2]*0.5f );

	indent(fp,depth*2); fprintf( fp, "axes:\n" );
	indent(fp,depth*2); fprintf( fp, "x(%.3f, %.3f, %.3f)\n", obb.Axis[0].x, obb.Axis[0].y, obb.Axis[0].z );
	indent(fp,depth*2); fprintf( fp, "y(%.3f, %.3f, %.3f)\n", obb.Axis[1].x, obb.Axis[1].y, obb.Axis[1].z );
	indent(fp,depth*2); fprintf( fp, "z(%.3f, %.3f, %.3f)\n", obb.Axis[2].x, obb.Axis[2].y, obb.Axis[2].z );

	if( pNode->pOBBNodeL ) DumpToTextFile( fp, pNode->pOBBNodeL, depth + 1 );
	if( pNode->pOBBNodeR ) DumpToTextFile( fp, pNode->pOBBNodeR, depth + 1 );
}

void OBBTree::GetLeafOBBs( std::vector<OBBDATA>& obbs )
{
	if( !pOBBTopNode )
		return;

	GetLeafOBBs( pOBBTopNode, obbs );
}

void OBBTree::DumpToTextFile( const std::string& filepath )
{
	if( !pOBBTopNode )
		return;

	FILE *fp = fopen( filepath.c_str(), "w" );
	if( !fp )
		return;

	DumpToTextFile( fp, pOBBTopNode, 0 );

	fclose(fp);
}

// The following source code was taken from NUMERICAL RECIPES in C
// http://www.library.cornell.edu/nr/bookcpdf/c11-1.pdf
#include <stdio.h>
#include <math.h>

void OBBTree::Rotate( float a[3][3], float s, float tau, int i, int j, int k, int l )
{
	float h, g;
	g = a[i][j];
	h = a[k][l];
	a[i][j] = g - s * ( h + g *tau );
	a[k][l] = h + s * ( g - h *tau );
}

bool OBBTree::Jacobi( float a[3][3], float v[3][3], float d[3] )
{
	int n = 3;
	int i, j, iq, ip;
	float tresh, theta, tau, t, sm, s, h, g, c, b[3], z[3];

	for( ip = 0; ip < n; ip++ ){
		for( iq = 0; iq < n; iq++ ) v[ip][iq] = 0.0f;
		v[ip][ip] = 1.0f;
	}
	for( ip = 0; ip < n; ip++ ){
		b[ip] = d[ip] = a[ip][ip];
		z[ip] = 0.0f;
	}
	for( i = 0; i < 50; i++ ){
		sm = 0.0f;
		for( ip = 0; ip < n - 1; ip++ ){
			for( iq = ip + 1; iq < n; iq++ ) sm += (float)fabs(a[ip][iq]);
		}

		if( sm == 0.0f ) return true;
		if( i < 3 ) tresh = 0.2f * sm / ( n * n );
		else tresh = 0.0f;
		for( ip = 0; ip < n - 1; ip++ ){
			for( iq = ip + 1; iq < n; iq++ ){
				g = 100.0f * (float)fabs( a[ip][iq] );
				if( i > 3 && ( fabs( d[ip] ) + g ) == fabs( d[ip] )
					&& ( fabs( d[iq] ) + g ) == fabs( d[iq] ) ) a[ip][iq] = 0.0f;
				else if( fabs( a[ip][iq] ) > tresh ){
					h = d[iq] - d[ip];
					if( ( fabs( h ) + g ) == fabs( h ) ) t = a[ip][iq] / h;
					else{
						theta = 0.5f * h / a[ip][iq];
						t = 1.0f / ( (float)fabs( theta ) + (float)sqrt( 1.0f + theta * theta ) );
						if( theta < 0.0f ) t = -t;
					}
					c = 1.0f / (float)sqrt( 1 + t * t );
					s = t * c;
					tau = s / ( 1.0f + c );
					h = t * a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq] = 0.0f;

					for( j = 0; j < ip; j++ ) Rotate( a, s, tau, j, ip, j, iq );
					for( j = ip + 1; j < iq; j++ ) Rotate( a, s, tau, ip, j, j, iq );
					for( j = iq + 1; j < n; j++ ) Rotate( a, s, tau, ip, j, iq, j );
					for( j = 0; j < n; j++ ) Rotate( v, s, tau, j, ip, j, iq );
				}
			}
		}
		for( ip = 0; ip < n; ip++ ){
			b[ip] += z[ip];
			d[ip] = b[ip];
			z[ip] = 0.0f;
		}
	}

	return false;
}


} // namespace amorphous
