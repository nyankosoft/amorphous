#version 330
uniform mat4 WorldITXf; // our four standard "untweakable" xforms
uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;
uniform mat4 ViewWorld;
uniform mat4 ProjViewWorld;
uniform mat4 ViewIXf;
uniform mat4 WvpXf;
uniform mat4 ShadowViewProjXf;
uniform vec3 SpotLightPos;
uniform float ShadBias = 0;

layout(location = 0) in vec4 position;
//layout(location = 3) in vec2 _UV;

layout(location = 0) out vec4 HPosition;
layout(location = 1) out vec4 LP;
layout(location = 2) out vec3 LightVec;
//layout(location = 3) out vec3 WNormal;
//layout(location = 4) out vec3 WView;
//layout(location = 5) out vec2 UV;

void main(void)
{
//	OUT.WNormal = mul(IN.Normal,WorldITXf).xyz; // world coords
	vec4 Po = vec4(position.x,position.y,position.z,1.0);     // "P" in object coordinates
	vec4 Pw = World * Po;                        // "P" in world coordinates
	vec4 Pl = ShadowViewProjXf * Pw;  // "P" in light coords
	Pl.z -= ShadBias;	// factor in bias here to save pixel shader work
	LP = Pl;                                                       
//	...for pixel-shader shadow calcs
//	OUT.WView = normalize(ViewIXf[3].xyz - Pw.xyz);     // world coords
//	OUT.HPosition = mul(Po,WvpXf);    // screen clipspace coords
	HPosition = ProjViewWorld * Po;
	gl_Position = HPosition;
//	UV = _UV.xy;                                                 
//	pass-thru
	// From the vertex position to the spot light source position
	LightVec = SpotLightPos - Pw.xyz;               // world coords
	//return OUT;
}
