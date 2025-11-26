#type compute
#version 430 core
#includeGlobalSource

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};

layout(std430, binding = 8) buffer buffer8
{
	bool shouldRedrawWorld; 
};

layout(std430, binding = 9) buffer buffer9
{
	bool shouldRedrawChunk[]; 
};
layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};
layout(std430, binding = 10) buffer buffer10
{
	bool shouldPlayFuseSound; 
};

uniform vec3 u_CameraPos;
uniform vec3 u_RayDirection;

#line 0
void main() {
	float step = 0.1;
	float distance = 15.0;
	vec3 startPosition = u_CameraPos;
	vec3 position = u_CameraPos;
	while(length(startPosition-position)<distance){
		position = position + u_RayDirection*step;
		uvec3 chunkPosition = uvec3(floor(position/16));
		vec3 actualChunkPosition = chunkPosition*16;
		ivec3 localPos = ivec3(position-chunkPosition*16);
		int chunkIndex = int(chunkPosition.x+chunkPosition.y*WORLD_WIDTH+chunkPosition.z*WORLD_WIDTH*WORLD_HEIGHT);
		if(chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]==tnt_block){
			vec3 blockOrigin = actualChunkPosition+localPos;	
			int relX = int(blockOrigin.x - DEFAULT_SPAWN.x-1);
			int relY = int(blockOrigin.y - SURFACE_LEVEL-1);
			int relZ = int(blockOrigin.z - DEFAULT_SPAWN.z-1);

			int tntIndex = relY * TNT_SIDE_LENGTH * TNT_SIDE_LENGTH + relZ * TNT_SIDE_LENGTH + relX;
			tnts[tntIndex].position = blockOrigin;
			tnts[tntIndex].velocity = vec3(0,6,0);
			tnts[tntIndex].visible = true;
			tnts[tntIndex].secondsUntilExplode = 4.0;
			shouldPlayFuseSound = true;
		}else{
			shouldPlayFuseSound = false;
		}
		if(chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]!=air){
			chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z] = 0;
			shouldRedrawWorld = true;
			shouldRedrawChunk[chunkIndex] = true;
			if (localPos.x == 0 && chunkPosition.x > 0)
				shouldRedrawChunk[getChunkIndex(chunkPosition.x - 1, chunkPosition.y, chunkPosition.z)] = true;
			else if (localPos.x == CHUNK_SIDE_LENGTH - 1)
				shouldRedrawChunk[getChunkIndex(chunkPosition.x + 1, chunkPosition.y, chunkPosition.z)] = true;

			if (localPos.y == 0 && chunkPosition.y > 0)
				shouldRedrawChunk[getChunkIndex(chunkPosition.x, chunkPosition.y - 1, chunkPosition.z)] = true;
			else if (localPos.y == CHUNK_SIDE_LENGTH - 1)
				shouldRedrawChunk[getChunkIndex(chunkPosition.x, chunkPosition.y + 1, chunkPosition.z)] = true;

			if (localPos.z == 0 && chunkPosition.z > 0)
				shouldRedrawChunk[getChunkIndex(chunkPosition.x, chunkPosition.y, chunkPosition.z - 1)] = true;
			else if (localPos.z == CHUNK_SIDE_LENGTH - 1)
				shouldRedrawChunk[getChunkIndex(chunkPosition.x, chunkPosition.y, chunkPosition.z + 1)] = true;
			break;
		}
	}
}
