#ifndef __PhysMeshData_HPP__
#define __PhysMeshData_HPP__


#include <vector>
#include "../base.hpp"
#include "fwd.hpp"


namespace amorphous
{


namespace physics
{

/*
class CMeshData
{
public:

	void *pPosBegin;
	uint PosStride;
	void *pNormalBegin;
	uint NormalStride;

public:

	CMeshData()
		:
	PosStride(0),
	NormalStride(0)
	{
	}
};
*/


class CStdMeshData
{
public:

	std::vector<Vector3> Positions;
	std::vector<Vector3> Normals;
	std::vector<int> Indices;
};


class CMeshData
{
public:

	void* pVerticesPosBegin;

	void* pVerticesNormalBegin;

	int VerticesPosByteStride;

	int VerticesNormalByteStride;

	U32 NumMaxVertices;

	U32* pNumVerticesPtr;

	void* pIndicesBegin;

	int IndicesByteStride;

	U32 NumMaxIndices;

	U32* pNumIndicesPtr;
/*
	void* parentIndicesBegin;

	I32	parentIndicesByteStride;

	U32	maxParentIndices;

	U32* numParentIndicesPtr;

	U32* dirtyBufferFlagsPtr;
*/
	U32 Flags;

//	const char*				name;			//!< Possible debug name. The string is not copied by the SDK, only the pointer is stored.

	inline ~CMeshData();

	inline void SetToDefault();

//	inline bool IsValid() const;

	inline	CMeshData();
};


inline CMeshData::CMeshData()
{
	SetToDefault();
}


inline CMeshData::~CMeshData()
{
}


inline void CMeshData::SetToDefault()
{
	pVerticesPosBegin		 = NULL;
	pVerticesNormalBegin	 = NULL;
	VerticesPosByteStride	 = 0;
	VerticesNormalByteStride = 0;
	NumMaxVertices			 = 0;
	pNumVerticesPtr			 = NULL;
	pIndicesBegin			 = NULL;
	IndicesByteStride		 = 0;
	NumMaxIndices			 = 0;
	pNumIndicesPtr			 = NULL;
/*	ParentIndicesBegin		 = NULL;
	ParentIndicesByteStride	 = 0;
	MaxParentIndices		 = 0;
	pNumParentIndicesPtr	 = NULL;
	pDirtyBufferFlagsPtr	 = NULL;*/
	Flags					 = 0;
//	name					 = NULL;
}



} // namespace physics


//================== inline implementations =======================
//#include "Joint.inl"

} // namespace amorphous



#endif  /*  __PhysMeshData_HPP__  */
