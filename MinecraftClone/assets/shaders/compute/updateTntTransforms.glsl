#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 65
#define WORLD_HEIGHT 16
#define BLOCKS_IN_CHUNK_COUNT CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH
#define FACES_PER_CHUNK BLOCKS_IN_CHUNK_COUNT
#define GRAVITY -28.57

layout (local_size_x = 2, local_size_y = 1, local_size_z = 1) in;

struct Chunk {
	int x;
	int y;
	int z;
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};
layout(std430, binding = 0) readonly buffer buffer0 
{
	Chunk chunksData[]; 
};

struct TntEntity{
    bool visible; 
	vec3 position;
	vec3 velocity;
};

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};
uniform float u_DeltaTime;

void main() {
	uint index = gl_WorkGroupID.x*2+gl_LocalInvocationIndex;
	vec3 chunkPosition = floor(tnts[index].position/16);
	vec3 actualChunkPosition = chunkPosition*16;
	ivec3 localPos = ivec3(tnts[index].position-chunkPosition*16);
	int chunkIndex = int(chunkPosition.x+chunkPosition.y*WORLD_WIDTH+chunkPosition.z*WORLD_WIDTH*WORLD_HEIGHT);
	if(chunkPosition.y>=WORLD_HEIGHT || chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]==0){
		tnts[index].velocity.y = tnts[index].velocity.y+GRAVITY*u_DeltaTime;
		tnts[index].position = tnts[index].position+tnts[index].velocity*u_DeltaTime;
	}else if(chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]!=0){
		//tnts[index].visible = false;
	}
}
