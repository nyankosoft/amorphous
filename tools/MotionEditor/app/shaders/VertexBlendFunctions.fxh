#ifndef __VERTEXBLENDFUNCTIONS_H__
#define __VERTEXBLENDFUNCTIONS_H__

//===========================================================================================
// SkinPoint
// Applies 4 matrix skinning to a single point.  The point passed in is changed as well as returned.
//
float4 SkinPoint( inout float4 io_value, float4x4 blendMats[NUM_MAX_BLEND_MATRICES],  int4 indices, float4 weights)
{
	int i;
	float4 incoming_io_value = io_value;

	if(weights[0] != -1 )
	{
		io_value = 0;
		// skin
//		for(i=0; i <4 ; i++)
		for(i=0; i <NUM_MAX_BLEND_MATRICES_PER_VERTEX ; i++)
		{
			io_value  += mul( incoming_io_value, blendMats[indices[3-i]]) * weights[i];
//			io_value  += mul( incoming_io_value, blendMats[indices[i]]) * weights[i];
		}
	}
	return io_value;
}

//===========================================================================================
// SkinVector
// Applies 4 matrix skinning to a vector.  The vector passed in is changed as well as returned.
//
float3 SkinVector( inout float3 io_value, float4x4 blendMats[NUM_MAX_BLEND_MATRICES], int4 indices, float4 weights)
{
	int i;
	float3 incoming_io_value = io_value;

	if(weights[0] != -1 )
	{
		io_value = 0;
    
		// skin
//		for(i=0; i <4 ; i++)
		for(i=0; i <NUM_MAX_BLEND_MATRICES_PER_VERTEX ; i++)
		{
			io_value  += mul( incoming_io_value, blendMats[indices[3-i]]) * weights[i];
//			io_value  += mul( incoming_io_value, blendMats[indices[i]]) * weights[i];
		}

		io_value = normalize(io_value);
	}

	return io_value;
}


#endif  /* __VERTEXBLENDFUNCTIONS_H__ */
