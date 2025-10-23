#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 65
#define WORLD_HEIGHT 16
#define BLOCKS_IN_CHUNK_COUNT CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH
#define FACES_PER_CHUNK BLOCKS_IN_CHUNK_COUNT

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 6) buffer buffer6
{
	vec3 tntPositions[];
};
layout(std430, binding = 7) buffer buffer7
{
	vec3 tntVelocities[];
};

void main() {
	uint index = gl_WorkGroupID.x + gl_LocalInvocationIndex;
    const int size = 100;

    uint xOffset = index % size;
    uint yOffset = (index / size) % size;
    uint zOffset = index / (size * size);
    tntPositions[index] = vec3(xOffset, yOffset, zOffset);

    float seed = float(index);

    float rand1 = fract(sin(seed * 12.9898) * 43758.5453);
    float rand2 = fract(sin((seed + 1.0) * 78.233) * 43758.5453);
    float rand3 = fract(sin((seed + 2.0) * 45.164) * 43758.5453);

    vec3 randomDir = normalize(vec3(rand1 * 2.0 - 1.0,rand2, rand3 * 2.0 - 1.0));

    float speed = 30; 
    tntVelocities[index] = randomDir * speed;
}
