#type vertex
#version 430 core

struct FaceData{
	uint packedPos;
};

layout(std430, binding = 0) readonly buffer vertexPullBuffer 
{
	FaceData packedMeshData[]; // Contains packed data for our vertices
};

const vec3 facePositions[4] = vec3[4]
(
	vec3(-0.5f, -0.5f, 0.0f),
	vec3(0.5f, -0.5f, 0.0f),
	vec3(0.5f, 0.5f, 0.0f),
	vec3(-0.5f, 0.5f, 0.0f)
);

int indices[6] = {0, 1, 2, 2, 3, 0};

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main()
{
	const int index = 0;
	const FaceData packedData = packedMeshData[index];
	const int currVertexID = gl_VertexID % 6;

	const uint x = (packedData.packedPos) & 1023;
	const uint y = (packedData.packedPos >> 10) & 1023;
	const uint z = (packedData.packedPos >> 20) & 1023;
	vec3 position = vec3(x, y, z);

	position += facePositions[indices[currVertexID]];

	gl_Position = u_ViewProjection*u_Transform*vec4(position, 1.0);
}




















#type fragment
#version 330 core

out vec4 color;
//in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	//color = texture(u_Texture,v_TexCoord);
	color = vec4(1.0f,0.0f,0.0f,1.0f);
}
