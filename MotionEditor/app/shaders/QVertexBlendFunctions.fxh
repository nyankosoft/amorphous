#ifndef __QVERTEXBLENDFUNCTIONS_FXH__
#define __QVERTEXBLENDFUNCTIONS_FXH__

//===========================================================================================
// SkinPoint
// Applies 4 matrix skinning to a single point.  The point passed in is changed as well as returned.
//
float4 SkinPoint( inout float4 io_value, Transform blends[NUM_MAX_BLEND_TRANSFORMS],  int4 indices, float4 weights)
{
	int i;
	float4 incoming_io_value = io_value;

	if(weights[0] != -1 )
	{
		io_value = 0;
		// skin
//		for(i=0; i <4 ; i++)
		for(i=0; i <NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX ; i++)
		{
			io_value  += mul( blends[indices[3-i]], incoming_io_value ) * weights[i];
//			io_value  += mul( blends[indices[i]], incoming_io_value ) * weights[i];
		}
	}
	return io_value;
}

//===========================================================================================
// SkinVector
// Applies 4 matrix skinning to a vector.  The vector passed in is changed as well as returned.
//
float3 SkinVector( inout float3 io_value, Transform blends[NUM_MAX_BLEND_TRANSFORMS], int4 indices, float4 weights)
{
	int i;
	float3 incoming_io_value = io_value;

	if(weights[0] != -1 )
	{
		io_value = 0;
    
		// skin
//		for(i=0; i <4 ; i++)
		for(i=0; i <NUM_MAX_BLEND_TRANSFORMS_PER_VERTEX ; i++)
		{
			io_value  += mul( blends[indices[3-i]], incoming_io_value ) * weights[i];
//			io_value  += mul( blends[indices[i]], incoming_io_value ) * weights[i];
		}

		io_value = normalize(io_value);
	}

	return io_value;
}


#endif  /* __QVERTEXBLENDFUNCTIONS_FXH__ */
