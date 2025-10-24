#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 65
#define WORLD_HEIGHT 16
#define BLOCKS_IN_CHUNK_COUNT CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH
#define FACES_PER_CHUNK BLOCKS_IN_CHUNK_COUNT
#define DEFAULT_SPAWN vec3(CHUNK_WIDTH * WORLD_WIDTH / 2, CHUNK_WIDTH * WORLD_HEIGHT*2, CHUNK_WIDTH * WORLD_WIDTH / 2)

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct TntEntity{
    bool visible; 
	vec3 position;
	vec3 velocity;
};

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};
vec3 hash33(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return fract((p.xxy + p.yzz) * p.zyx);
}
void main() {
	uint index = gl_WorkGroupID.x + gl_LocalInvocationIndex;
    const int size = 171;

    uint xOffset = index % size;
    uint yOffset = (index / size) % size;
    uint zOffset = index / (size * size);
    tnts[index].position = DEFAULT_SPAWN + vec3(xOffset, yOffset, zOffset);

    float seed = float(index);

	vec3 rand = hash33(vec3(index, index + 1.0, index + 2.0));
	vec3 randomDir = normalize(rand * 2.0 - 1.0);

    float speed = 35; 
    tnts[index].velocity = randomDir * speed;
//    if(index>(1000000-2)){ 
//		tnts[index].visible = true;
//    }else{
//		tnts[index].visible = false;
//    }
//
	tnts[index].visible = true;
}
