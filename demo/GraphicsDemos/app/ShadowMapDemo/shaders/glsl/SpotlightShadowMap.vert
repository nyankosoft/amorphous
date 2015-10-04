#version 330
layout(location = 0) in vec4 position;
layout(location = 0) out vec4 _p;
layout(location = 1) out vec4 light_space_position;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;
uniform mat4 ViewWorld;
uniform mat4 ProjViewWorld;

//uniform mat4 ProjLView;
//uniform mat4 ProjLViewWorld;


void main(void)
{
	vec4 Po = vec4(position.x,position.y,position.z,1.0);     // object coordinates
	vec4 Pw = World * Po;                        // "P" in world coordinates
//	vec4 Pl = ProjView * Pw;  // "P" in light coords
	vec4 Pl = ProjViewWorld * Po;
//	OUT.LP = Pl;                // view coords (also lightspace projection coords in this case)
//	OUT.HPosition = Pl; // screen clipspace coords

	_p = Pl;
	light_space_position = Pl;
//	light_space_position = Pw;

	gl_Position = Pl;
}
