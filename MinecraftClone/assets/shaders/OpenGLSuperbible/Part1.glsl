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

	gl_Position = vertices[gl_VertexID] + offset;
	vs_out.color = color;

}

#type tessellationControl
#version 450 core
layout (vertices = 3) out;

void main(){
	if(gl_InvocationID == 0){
		gl_TessLevelInner[0] = 16.0;
		gl_TessLevelOuter[0] = 16.0;
		gl_TessLevelOuter[1] = 16.0;
		gl_TessLevelOuter[2] = 16.0;
	}
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}

#type tesselationEvaluation
#version 450 core
layout (triangles, equal_spacing, cw) in;

void main(){
	gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position+
		gl_TessCoord.y * gl_in[1].gl_Position+
		gl_TessCoord.z * gl_in[2].gl_Position);
}


#type fragment
#version 450 core 

// in VS_OUT
// {
// 	vec4 color;
// } fs_in;
out vec4 color;

void main() { 
	color = vec4(0.0f, 0.8f, 1.0f, 1.0f);
}
