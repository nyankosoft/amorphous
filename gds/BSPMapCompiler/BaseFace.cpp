#include "BaseFace.hpp"
#include "3DMath/ray.hpp"
#include "3DMath/Triangle.hpp"
#include "Graphics/FloatRGBColor.hpp"
#include "Stage/bspstructs.hpp"


//=====================================================================================
// CPlaneBuffer::Methods()                                        - class CPlaneBuffer
//=====================================================================================

//Returns the index to the plane 
bool CPlaneBuffer::AddPlaneFromFace(CFace& rFace)
{
	SPlane plane;

	//Create new plane from rFace
	if( rFace.MakePlane(plane) )
	{	//The plane was successfully created from rFace
		short sIndex = AddPlane(plane);

		rFace.m_pPlaneBuffer = this;
		rFace.m_sPlaneIndex = sIndex;
		return true;
	}
	else
	{	//The plane was not created properly from rFace
		//This occurs when all the vertices of 'rFace' is on a straight line
		return false;

	}
}

short CPlaneBuffer::AddPlane(SPlane &rPlane)
{
	short i=0;
	CPlaneBuffer::iterator p = begin();

	while(p != end())
	{
		//if a plane in the buffer is overlapping with the pFace, return its index
		if( fabs(p->normal.x - rPlane.normal.x) < NORMAL_EPSILON
			&& fabs(p->normal.y - rPlane.normal.y) < NORMAL_EPSILON
			&& fabs(p->normal.z - rPlane.normal.z) < NORMAL_EPSILON
			&& fabs(p->dist  - rPlane.dist) < DIST_EPSILON )
			return i;
		p++;
		i++;
	}

	//Add the newly created plane to this buffer
	push_back(rPlane);
	return ( size() - 1 );
}


//=====================================================================================
// CFace::Methods()                                                      - class CFace
//=====================================================================================

CFace::CFace()
{
	m_pVertices.reserve(NUM_VERTICES_PER_FACE);
	m_sPlaneIndex = 0;
	m_pPlaneBuffer = NULL;

	m_AABB.Nullify();
}


CFace::CFace(const CFace& face)
{
	this->m_pVertices.assign( face.m_pVertices.begin(), face.m_pVertices.end() );
	this->m_pPlaneBuffer = face.m_pPlaneBuffer;
	this->m_sPlaneIndex = face.m_sPlaneIndex;

	this->m_AABB = face.m_AABB;
}


CFace CFace::operator =(CFace face)
{
	this->m_pVertices.clear();
	this->m_pVertices.assign( face.m_pVertices.begin(), face.m_pVertices.end() );

	this->m_sPlaneIndex = face.m_sPlaneIndex;
	this->m_pPlaneBuffer = face.m_pPlaneBuffer;
	this->m_AABB = face.m_AABB;

	return *this;
}


void CFace::SetLightmapUV(short i, float u, float v)
{
	this->m_pVertices[i].vTex1.u = u;
	this->m_pVertices[i].vTex1.v = v;
}


void CFace::SetPlaneTo(SNode& rNode) // copy plane data to a node
{
	rNode.pPlaneBuffer = m_pPlaneBuffer;
	rNode.sPlaneIndex = m_sPlaneIndex;
}


void CFace::SetPlaneFrom(SNode& rNode) // Copy plane data from a node to this face
{
	m_pPlaneBuffer = rNode.pPlaneBuffer;
	m_sPlaneIndex = rNode.sPlaneIndex;
}


//  If a polygon has more than 4 vertices, some of its vertices may
//be on a straight line.
bool CFace::MakePlane( SPlane& rPlane )
{
	Vector3 p0,p1,out;
	Vector3 normal(0.0f, 0.0f, 0.0f);
	Vector3 vZeroVector(0.0f, 0.0f, 0.0f);
	float dist;
	int i=0,j,k;
	int iNumPnts = (int)m_pVertices.size();
	while( (fabs(normal.x) < NORMAL_EPSILON)
		&& (fabs(normal.y) < NORMAL_EPSILON)
		&& (fabs(normal.z) < NORMAL_EPSILON)
		&& (i < iNumPnts) )
	{
	 
		j = (i+1) % iNumPnts;
		k = (i+2) % iNumPnts;
		p0 = GetVertex(i) - GetVertex(j);
		p1 = GetVertex(k) - GetVertex(j);
		Vec3Cross( normal, p1, p0 );
		Vec3Normalize( out, normal );
		i++;
	}

	if(normal == vZeroVector)
	{
		//MessageBox(NULL, "Plane-normal was not created properly.","Error",MB_OK | MB_ICONWARNING);
		return false;
	}

	normal = out;
	dist = Vec3Dot( GetVertex(0), normal );   //Distance between (0,0,0) and this plane

	rPlane.normal = normal;
	rPlane.dist = dist;

	if( fabs( fabs(normal.x) - 1.0 ) < NORMAL_EPSILON )
		rPlane.type = 0;	//rPlane is perpendicular to x-axis
	else if( fabs( fabs(normal.y) - 1.0 ) < NORMAL_EPSILON )
		rPlane.type = 1;	//rPlane is perpendicular to y-axis
	else if( fabs( fabs(normal.z) - 1.0 ) < NORMAL_EPSILON )
		rPlane.type = 2;	//rPlane is perpendicular to z-axis
	else if( normal.y == 0.0f )
		rPlane.type = 3;	//rPlane is vertical (y == 0)
	else
		rPlane.type = 5;

	return true;
}


bool CFace::HasSamePlaneWith(CFace& rFace)
{
	if(this->m_sPlaneIndex == rFace.m_sPlaneIndex
		&& this->m_pPlaneBuffer == rFace.m_pPlaneBuffer)
		return true;
	else
		return false;
}


float CFace::CalculateArea()
{
	short i;
	float fCos, fSin;
	Vector3 vEdge1, vEdge2;
	float fEdgeLen1, fEdgeLen2;
	float fArea = 0.0f;

	int iNumVertices = this->GetNumVertices();
	for(i=1; i<iNumVertices - 1; i++)
	{
		vEdge1 = GetVertex(i) - GetVertex(0);
		vEdge2 = GetVertex(i+1) - GetVertex(0);
		fEdgeLen1 = Vec3Length(vEdge1);
		fEdgeLen2 = Vec3Length(vEdge2);
		Vec3Normalize(vEdge1, vEdge1);
		Vec3Normalize(vEdge2, vEdge2);
		fCos = Vec3Dot(vEdge1, vEdge2);
		fSin = sqrtf( 1.0f - fCos * fCos );
		
		fArea += fEdgeLen1 * fEdgeLen2 * fSin / 2.0f;
	}

	return fArea;
}

//Returns true if this face has at least one some point with the 'rFace'
bool CFace::SharingPointWith(CFace& rFace)
{
	short i,j;
	int iNumVertices = this->GetNumVertices();
	int jNumVertices = rFace.GetNumVertices();

	for(i=0; i<iNumVertices; i++)
	{
		Vector3& p1 = GetVertex(i);
		for(j=0; j<jNumVertices; j++)
		{
			Vector3& p2 = rFace.GetVertex(j);
			if( p1 == p2 )
				return true;
		}
	}
	return false;
}


void CFace::Split(CFace& front, CFace& back, SPlane& cutplane)
{
	unsigned int j;
	unsigned int uiNumVertices = m_pVertices.size();  //the number of points of this face
	SFloatRGBColor col, col0, col1;

	//Initialization for 'front' and 'back': delete the vertices and copy the plane from the original face
	front = *this;
	back = *this;
	front.m_pVertices.clear();
	back.m_pVertices.clear();
	front.m_pVertices.reserve( uiNumVertices + 1 ); //Split faces will have almost the same number of points
	back.m_pVertices.reserve( uiNumVertices + 1 );
	

	//check if each edge of this face and the 'cutplane' intersect
	for( j=0; j<uiNumVertices; j++)
	{
		MAPVERTEX& mv0 = m_pVertices[j];
		MAPVERTEX& mv1 = m_pVertices[(j+1) % uiNumVertices];
		Vector3& p0 = mv0.vPosition;      //face に含まれる頂点(Vector3)を任意に２つ選び
		Vector3& p1 = mv1.vPosition;      //その２頂点とplaneの関係を調べる 
		int c0 = ClassifyPoint( cutplane, p0 );
		int c1 = ClassifyPoint( cutplane, p1 );
		if( ( c0 == PNT_FRONT && c1 == PNT_BACK ) 	// need split edge
		||  ( c0 == PNT_BACK && c1 == PNT_FRONT ) )
		{
			float d0 = Vec3Dot( p0, cutplane.normal ) - cutplane.dist;
			float d1 = Vec3Dot( p1, cutplane.normal ) - cutplane.dist;
			float f = - d0 / ( d1 - d0 );
			Vector3 pn = p0 + ( p1 - p0 ) * f;  //the cross point between linesegment (p1 - p0) and cutplane
			MAPVERTEX vNew;
			vNew.vPosition = pn;
			vNew.vTex0.u = mv0.vTex0.u + (mv1.vTex0.u - mv0.vTex0.u) * f;
			vNew.vTex0.v = mv0.vTex0.v + (mv1.vTex0.v - mv0.vTex0.v) * f;
//			col0.CopyFromD3DCOLOR( mv0.color );
//			col1.CopyFromD3DCOLOR( mv1.color );
			col0.SetARGB32( mv0.color );
			col1.SetARGB32( mv1.color );
			col = ( col0 + (col1 - col0) * f );
//			vNew.color = col.ConvertToD3DCOLOR();
			vNew.color = col.GetARGB32();
			vNew.vTex1.u = mv0.vTex1.u + (mv1.vTex1.u - mv0.vTex1.u) * f;
			vNew.vTex1.v = mv0.vTex1.v + (mv1.vTex1.v - mv0.vTex1.v) * f;

			Vector3 vTempNormal = mv0.vNormal + (mv1.vNormal - mv0.vNormal) * f;
			Vec3Normalize( vNew.vNormal, vTempNormal );

			if( c0 == PNT_FRONT )
				front.m_pVertices.push_back( mv0 );  //Add P0 vertex to CFace front
			if( c0 == PNT_BACK )
				back.m_pVertices.push_back( mv0 );
			front.m_pVertices.push_back(vNew);
			back.m_pVertices.push_back(vNew);
			continue;
		}
		if( c0 == PNT_FRONT ) front.m_pVertices.push_back( mv0 );	//front -> on
		if( c0 == PNT_BACK  ) back.m_pVertices.push_back( mv0 );		//back -> on
		if( c0 == PNT_ONPLANE )
		{
			front.m_pVertices.push_back( mv0 );
			back.m_pVertices.push_back( mv0 );
		}
	}
}

//
//	faceを分断平面でクリップします。
//	平面上のfaceと、表のfaceは生き残り。
//	裏に位置するfaceは捨てられます。
void CFace::ClipFaceWithPlane(SPlane& rPlane)
{
	CFace frontface = *this;
	CFace backface = *this;
	switch( ClassifyFace( rPlane, *this ) )
	{
	case FCE_ONPLANE:
	case FCE_FRONT:
		break;			// non clip
	case FCE_BACK:
		this->ClearVertices();	//clipped out
		break;
	case FCE_SPLIT:
		Split(frontface, backface, rPlane);
		*this = frontface;
		break;
	default:
		;
	}
}

#define BIG_NUM 10000.0f
void CFace::MakeLargeFaceFromPlane()
{
	SPlane& rPlane = GetPlane();
	float dist = rPlane.dist;
	Vector3 vNormal = rPlane.normal;

	Vector3 vUP = Vector3( 0.0f, 1.0f, 0.0f );
	if( fabs(vNormal.x) < 0.001 && fabs(vNormal.z) < 0.001 )
	{//the plane is almost horizontal
		//illegal case
		vUP = Vector3( -vNormal.y, 0.0, 0.0 );
	}
	Vector3 vRight;
	Vec3Cross( vRight, vUP, vNormal );
	Vec3Normalize( vRight, vRight );
	Vec3Cross( vUP, vNormal, vRight );
	Vec3Normalize( vUP, vUP );
	
	Vector3 vOrig = vNormal * dist;
	vUP *= BIG_NUM;
	vRight *= BIG_NUM;
	
	AddVertex( vOrig + vUP + vRight );
	AddVertex( vOrig + vUP - vRight );
	AddVertex( vOrig - vUP - vRight );
	AddVertex( vOrig - vUP + vRight );
}

void CFace::AddToAABB(AABB3& aabb)
{
	Vector3& vMin = aabb.vMin;
	Vector3& vMax = aabb.vMax;

	int j;
	for(j=0; j<GetNumVertices(); j++)
	{
		Vector3& v = GetVertex( j );
		if (v.x < vMin.x) vMin.x = v.x;
		if (v.y < vMin.y) vMin.y = v.y;
		if (v.z < vMin.z) vMin.z = v.z;
		if (vMax.x < v.x) vMax.x = v.x;
		if (vMax.y < v.y) vMax.y = v.y;
		if (vMax.z < v.z) vMax.z = v.z;
	}
}

// clip CFace instance
bool CFace::ClipVisibility(CFace& rSrcFace, CFace& rDestFace, int iClipStyle)
{
	Vector3 v1,v2;
	SPlane clipplane;
	int i,j, iNumPnts = rSrcFace.GetNumVertices();
	int c;
	for(i=0; i<iNumPnts; i++)
	{
		v1 = rSrcFace.GetVertex( (i+1) % iNumPnts ) - rSrcFace.GetVertex(i);
		for(j=0; j<rDestFace.GetNumVertices(); j++)
		{
			v2 = rDestFace.GetVertex(j) - rSrcFace.GetVertex(i);
			Vec3Cross( clipplane.normal, v1, v2 );
			if( Vec3Length(clipplane.normal) < DIST_EPSILON )
				continue;
			Vec3Normalize( clipplane.normal, clipplane.normal );
			clipplane.dist = Vec3Dot( rDestFace.GetVertex(j), clipplane.normal );
			c = ClassifyFace( clipplane, rSrcFace );
			if(c == FCE_ONPLANE)
				continue;
			//clipstyleがCLIP_NORMALの時、作った平面の裏側にrSrcFaceがあるようにする
			//clipstyleがCLIP_INVERTの時、作った平面の表側にrSrcFaceがあるようにする
			if( c != (iClipStyle==CLIP_NORMAL?FCE_BACK:FCE_FRONT) )
				clipplane.Flip();

			//iClipStyleがCLIP_NORMALの時、rDestFaceが、表側にある時、その平面の表側が見える最大範囲
			//iClipStyleがCLIP_INVERTの時、rDestFaceが、裏側にある時、その平面の表側が見える最大範囲
			c = ClassifyFace( clipplane, rDestFace );
			if( c == (iClipStyle==CLIP_NORMAL?FCE_FRONT:FCE_BACK) )
			{
				this->ClipFaceWithPlane( clipplane );
				if( this->GetNumVertices() == 0 ) return false;	//clipped out
			}
		}
	}
	return true;
}


bool CFace::ClipTrace( CLineSegment& line_segment, CLineSegmentHit& results )
{
	bool b;
	int i, iNumTris = GetNumVertices() - 2;
	Vector3 vStart = line_segment.vStart;
	Vector3 vGoal  = line_segment.vGoal;
	Vector3 vOrigTrace = vGoal - vStart;

	// check triangles in this convex polygon
	for( i=0; i<iNumTris; i++ )
	{
//		CTriangle triangle( GetVertex(i), GetVertex(i+1), GetVertex(i+2), GetPlane().normal );
		CTriangle triangle( GetVertex(0), GetVertex(i+1), GetVertex(i+2), GetPlane().normal );

		b = triangle.RayIntersect( vStart, vGoal );

		if( b )
		{
			results.vEnd = vGoal;
			results.fFraction = Vec3Dot( results.vEnd - line_segment.vStart, vOrigTrace ) / Vec3LengthSq( vOrigTrace );
			return true;
		}
	}

	return false;
}