#type vertex
#version 450 core

void main() { 

	const vec4 vertices[3] = vec4[3]( vec4(0.25,-0.25,0.5,1.0),
			vec4(-0.25,-0.25,0.5,1.0), vec4(0.25,0.25,0.5,1.0));

	gl_Position = vertices[gl_VertexID];

}


#type fragment
#version 450 core 

out vec4 FragColor;

void main() { FragColor = vec4(0.0,0.8,1.0,1.0); } 
