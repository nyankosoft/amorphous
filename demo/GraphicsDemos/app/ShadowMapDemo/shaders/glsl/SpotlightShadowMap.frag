#version 330
layout(location = 1) in vec4 light_space_position;
layout(location = 0) out vec4 fc;

void main(void)
{
	float z = light_space_position.z;
//	gl_FragColor = vec4(z,z,z,1.0);
//	gl_FragColor = vec4(1,0,0,1);
//	fc = vec4(1,0,0,1);
	fc = vec4(z*0.05, z*0.05, z*0.05, 1.0);
}
