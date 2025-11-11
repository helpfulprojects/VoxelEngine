#type vertex
#version 450 core
layout (location = 0) in vec4 offset;
layout (location = 1) in vec4 color;

out VS_OUT
{
	vec4 color;
} vs_out;

void main() { 

	const vec4 vertices[3] = vec4[3]( vec4(0.25,-0.25,0.5,1.0),
			vec4(-0.25,-0.25,0.5,1.0), vec4(0.25,0.25,0.5,1.0));

	const vec4 colors[3] = vec4[3]( vec4(1,0,0,1),
				vec4(0,1,0,1),vec4(0,0,1,1));


	gl_Position = vertices[gl_VertexID];
	vs_out.color = colors[gl_VertexID];

}


#type fragment
#version 450 core 

in VS_OUT
{
	vec4 color;
} fs_in;
out vec4 color;

void main() { 
	color = fs_in.color;
}
