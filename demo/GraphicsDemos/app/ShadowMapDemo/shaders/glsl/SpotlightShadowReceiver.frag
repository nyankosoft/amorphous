#version 330
//layout(location = 1) in vec4 light_space_position;
layout(location = 0) out vec4 fc;

uniform float LightSize = 0.1;
uniform float SceneScale = 1.0;
uniform float ShadBias = 0;

layout(location = 0) in vec4 HPosition;
layout(location = 1) in vec4 LP;
layout(location = 2) in vec3 LightVec;
//layout(location = 3) in vec3 WNormal;
//layout(location = 4) in vec3 WView;

uniform sampler2D ShadowMapSampler;


/*********************************************************/
/*********** pixel shader ********************************/
/*********************************************************/

//vec4 shadPS(JustShadowVertexOutput IN) : COLOR
//{
//   // IN.LP.z = distance from the light source measured in the direction of the light?
//   return vec4(IN.LP.zzz,1);
//}

// -------------------------------------
// STEP 1: Search for potential blockers
// -------------------------------------
/**
 \return an averaged texel value sampled from the shadow map texture 
 *//*
float findBlocker(vec2 uv,
		vec4 LP,
		uniform sampler2D ShadowMap,
		uniform float bias,
		float searchWidth,
		float numSamples)
{
        // divide filter width by number of samples to use
        float stepSize = 2 * searchWidth / numSamples;

        // compute starting point uv coordinates for search
        uv = uv - vec2(searchWidth, searchWidth);

        // reset sum to zero
        float blockerSum = 0;
        float receiver = LP.z;
        float blockerCount = 0;
        float foundBlocker = 0;

        // iterate through search region and add up depth values
        for (int i=0; i<numSamples; i++) {
               for (int j=0; j<numSamples; j++) {
                       float shadMapDepth = tex2D(ShadowMap, uv +
                                                 vec2(i*stepSize,j*stepSize)).x;
                       // found a blocker
                       if (shadMapDepth < receiver) {
                               blockerSum += shadMapDepth;
                               blockerCount++;
                               foundBlocker = 1;
                       }
               }
        }

		float result;
		
		if (foundBlocker == 0) {
			// set it to a unique number so we can check
			// later to see if there was no blocker
			result = 999;
		}
		else {
		    // return average depth of the blockers
			result = blockerSum / blockerCount;
		}
		
		return result;
}

// ------------------------------------------------
// STEP 2: Estimate penumbra based on
// blocker estimate, receiver depth, and light size
// ------------------------------------------------
float estimatePenumbra(vec4 LP,
			float Blocker,
			uniform float LightSize)
{
       // receiver depth
       float receiver = LP.z;
       // estimate penumbra using parallel planes approximation
       float penumbra = (receiver - Blocker) * LightSize / Blocker;
       return penumbra;
}
*/
// ----------------------------------------------------
// Step 3: Percentage-closer filter implementation with
// variable filter width and number of samples.
// This assumes a square filter with the same number of
// horizontal and vertical samples.
// ----------------------------------------------------

float PCF_Filter(vec2 uv, vec4 LP, /*uniform*/ sampler2D ShadowMap, 
                /*uniform*/ float bias, float filterWidth, float numSamples)
{
	// compute step size for iterating through the kernel
	float stepSize = 2 * filterWidth / numSamples;

	// compute uv coordinates for upper-left corner of the kernel
	uv = uv - vec2(filterWidth,filterWidth);

	float sum = 0;  // sum of successful depth tests

       // now iterate through the kernel and filter
       for(int i=0; i<numSamples; i++) {
               for(int j=0; j<numSamples; j++) {
                       // get depth at current texel of the shadow map
                       float shadMapDepth = 0;
                       
                       shadMapDepth = texture(ShadowMap, uv + vec2(i*stepSize,j*stepSize)).x;

                       // test if the depth in the shadow map is closer than
                       // the eye-view point
					   // float shad = LP.z < shadMapDepth; // Original HLSL code
					   float shad = LP.z < shadMapDepth ? 1.0 : 0.0;

                       // accumulate result
                       sum += shad;
               }
       }
       
	// return average of the samples
	return sum / (numSamples*numSamples);
}

//uniform vec3 SpotLightColor,
//uniform float LightSize,
//uniform float SceneScale,
//uniform float ShadBias,
//uniform float Kd,
//uniform vec3 SurfColor,
//uniform sampler2D ShadowMapSampler,
//uniform sampler2D FloorSampler) : COLOR

void main(void)
{
   // Generic lighting code 
   //vec3 Nn = normalize(WNormal);
   //vec3 Vn = normalize(WView);
   //vec3 Ln = normalize(LightVec);
   //float ldn = dot(Ln,Nn);
   //vec3 diffContrib = SurfColor*(Kd*ldn * SpotLightColor);
   // vec3 result = diffContrib;
	vec3 diffContrib = vec3(1,1,1);

	// The soft shadow algorithm follows:

	// Compute uv coordinates for the point being shaded
	// Saves some future recomputation.
	vec2 uv = vec2(.5,-.5)*(LP.xy)/LP.w + vec2(.5,.5);

	// ---------------------------------------------------------
	// Step 1: Find blocker estimate
	float searchSamples = 6;   // how many samples to use for blocker search
	float zReceiver = LP.z;
	float searchWidth = SceneScale * (zReceiver - 1.0) / zReceiver;
	float blocker = 0;// findBlocker(uv, LP, ShadowMapSampler, ShadBias,
                              //SceneScale * LightSize / IN.LP.z, searchSamples);
   
	//return (blocker*0.3);  // uncomment to visualize blockers
   
	// ---------------------------------------------------------
	// Step 2: Estimate penumbra using parallel planes approximation
	float penumbra;  
	penumbra = 0;// estimatePenumbra(LP, blocker, LightSize);

	//return penumbra*32;  // uncomment to visualize penumbrae

	// ---------------------------------------------------------
	// Step 3: Compute percentage-closer filter
	// based on penumbra estimate
	float samples = 8;	// reduce this for higher performance

	// Now do a penumbra-based percentage-closer filter
	float shadowed; 

//	shadowed = 1.0;
	shadowed = PCF_Filter(uv, LP, ShadowMapSampler, ShadBias, penumbra, samples);

	// If no blocker was found, just return 1.0
	// since the point isn't occluded
   
	if (blocker > 998) 
		shadowed = 1.0;

	// Visualize lighting and shadows

	vec3 floorColor = vec3(1, 1, 1);// tex2D(FloorSampler, IN.UV * 2).rgb;
	//fc = floorColor;
	//fc = shadowed;

	fc = vec4((shadowed*diffContrib*floorColor),1);
}
