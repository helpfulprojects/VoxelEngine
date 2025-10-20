#type compute
#version 430 core

#define CHUNK_WIDTH 10
#define WORLD_WIDTH 16
#define WORKGROUP_SIZE_X 10

layout (local_size_x = WORKGROUP_SIZE_X, local_size_y = WORKGROUP_SIZE_X, local_size_z = WORKGROUP_SIZE_X) in;

struct Chunk {
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

layout(std430, binding = 0) readonly buffer buffer0 
{
	Chunk chunksData[WORLD_WIDTH][WORLD_WIDTH][WORLD_WIDTH]; 
};

void main() {
//    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
//    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
//    float speed = 100;
//    float width = 1000;
//	
//    //value.x = float(texelCoord.x)/(gl_NumWorkGroups.x*gl_WorkGroupSize.x);
//    value.x = mod(float(texelCoord.x)+t*speed,width)/(gl_NumWorkGroups.x*gl_WorkGroupSize.x);
//    value.y = float(texelCoord.y)/(gl_NumWorkGroups.y*gl_WorkGroupSize.y);
//	
//    imageStore(imgOutput, texelCoord, value);
}