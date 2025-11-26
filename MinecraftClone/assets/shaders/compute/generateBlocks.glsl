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

void main() {
	uint chunkIndex = getChunkIndex(gl_WorkGroupID.x,gl_WorkGroupID.y,gl_WorkGroupID.z);
	shouldRedrawWorld = true;
	shouldRedrawChunk[chunkIndex] = true;
	int index = 0;

	float freq = 0.005;     
	float amplitude = 20.0;     
	float baseHeight = 150.0;

	for(int x=0;x<CHUNK_SIDE_LENGTH;x++){
		for(int z=0;z<CHUNK_SIDE_LENGTH;z++){
			uint chunkX = gl_WorkGroupID.x * CHUNK_SIDE_LENGTH;
			uint chunkZ = gl_WorkGroupID.z * CHUNK_SIDE_LENGTH;
			chunksData[chunkIndex].x = chunkX;
			chunksData[chunkIndex].z = chunkZ;
			uint blockX = x+chunkX;
			uint blockZ = z+chunkZ;
			int surfaceLevel = SURFACE_LEVEL;

			for(int y=0;y<CHUNK_SIDE_LENGTH;y++){
				uint chunkY = gl_WorkGroupID.y * CHUNK_SIDE_LENGTH;
				uint blockY = y+chunkY;
				chunksData[chunkIndex].y = chunkY;
				if(blockY == 0){
					chunksData[chunkIndex].blockTypes[x][y][z] = bedrock_block;
				} else if(blockY<surfaceLevel){
					if(blockY<surfaceLevel-3){
						chunksData[chunkIndex].blockTypes[x][y][z] = stone_block;
					}else{
						chunksData[chunkIndex].blockTypes[x][y][z] = dirt_block;
					}
				}else if (blockY==surfaceLevel){
					chunksData[chunkIndex].blockTypes[x][y][z] = grass_block;
				} else{
					chunksData[chunkIndex].blockTypes[x][y][z] = air;
				}
				if(blockY>surfaceLevel && blockY <= surfaceLevel+TNT_SIDE_LENGTH &&
						blockX > DEFAULT_SPAWN.x && blockX <= DEFAULT_SPAWN.x+TNT_SIDE_LENGTH &&
						blockZ > DEFAULT_SPAWN.z && blockZ <= DEFAULT_SPAWN.z+TNT_SIDE_LENGTH){
						chunksData[chunkIndex].blockTypes[x][y][z] = tnt_block;
				}

				chunksData[chunkIndex].explosions[x][y][z] = 0;
				chunksData[chunkIndex].hasExplosion = false;
			}
		}
	}
}
