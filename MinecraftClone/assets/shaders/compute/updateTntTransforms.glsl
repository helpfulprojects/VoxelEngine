#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 65
#define WORLD_HEIGHT 16
#define BLOCKS_IN_CHUNK_COUNT CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH
#define FACES_PER_CHUNK BLOCKS_IN_CHUNK_COUNT
#define GRAVITY -28.57

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

layout(std430, binding = 6) buffer buffer6
{
	vec3 tntPositions[];
};
layout(std430, binding = 7) buffer buffer7
{
	vec3 tntVelocities[];
};
uniform float u_DeltaTime;

void main() {
	uint index = gl_WorkGroupID.x+gl_LocalInvocationIndex;
	vec3 chunkPosition = floor(tntPositions[index]/16);
	vec3 actualChunkPosition = chunkPosition*16;
//	int x = int(mod(tntPositions[index].x,16));
//	int y = int(mod(tntPositions[index].y,16));
//	int z = int(mod(tntPositions[index].z,16));

	ivec3 localPos = ivec3(tntPositions[index]-chunkPosition*16);
	int chunkIndex = int(chunkPosition.x+chunkPosition.y*WORLD_WIDTH+chunkPosition.z*WORLD_WIDTH*WORLD_HEIGHT);
//	if(chunkPosition.y>=WORLD_HEIGHT || chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]==0){
//		tntVelocities[index].y = tntVelocities[index].y+GRAVITY*u_DeltaTime;
//	}else{
//		tntVelocities[index] = vec3(0,0,0);
//	}

	//if(chunkPosition.y>=WORLD_HEIGHT || actualChunkPosition.x != chunksData[chunkIndex].x){
	//if(chunkPosition.y!=WORLD_HEIGHT){
	//if(tntPositions[index].y>256){
	if(chunkPosition.y>=WORLD_HEIGHT || chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]==0){
		tntVelocities[index].y = tntVelocities[index].y+GRAVITY*u_DeltaTime;
		tntPositions[index] = tntPositions[index]+tntVelocities[index]*u_DeltaTime;
	}
}
