#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 2
#define WORKGROUP_SIZE_X 1

layout (local_size_x = WORKGROUP_SIZE_X, local_size_y = WORKGROUP_SIZE_X, local_size_z = WORKGROUP_SIZE_X) in;

layout(std430, binding = 4) buffer buffer4
{
	uvec3 debugBuffer[];
};

void main() {
	uvec3 test = gl_WorkGroupID;
	uint groupIndex = test.x + test.y*2 + test.z*2*2;
	debugBuffer[groupIndex] = gl_WorkGroupID;
}
