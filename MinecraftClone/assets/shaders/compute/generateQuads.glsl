#type compute

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};

layout(std430, binding = 1) buffer buffer1 
{
	ChunkQuads chunksQuads[]; 
};
layout(std430, binding = 4) buffer buffer4
{
	uint debugBuffer[];
};
layout(std430, binding = 5) buffer buffer5
{
	uint renderData[];
};
layout(std430, binding = 8) buffer buffer8
{
	bool shouldRedrawWorld; 
};

layout(std430, binding = 9) buffer buffer9
{
	bool shouldRedrawChunk[]; 
};

ivec3 offsets[6] = ivec3[6](
    ivec3(0, 1, 0),
    ivec3(0, -1, 0),
    ivec3(1, 0, 0),
    ivec3(-1, 0, 0),
    ivec3(0, 0, 1),
    ivec3(0, 0, -1)
);


void main() {
	uint chunkIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*WORLD_WIDTH+gl_WorkGroupID.z*WORLD_WIDTH*WORLD_HEIGHT;
	if(!shouldRedrawWorld){
		return;
	}
	if(!shouldRedrawChunk[chunkIndex]){
		//renderData[chunkIndex] = 0;
		return;	
	}
	//chunksData[chunkIndex].shouldRedraw = false;
	//renderData[chunkIndex] = 0;
//	for (int i = 0; i < FACES_PER_CHUNK; ++i) {
//		chunksQuads[chunkIndex].blockQuads[i] = 0u;
//	}
//
	int index = 0;
	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int y=0;y<CHUNK_WIDTH;y++){
			for(int z=0;z<CHUNK_WIDTH;z++){
				uint blockType = chunksData[chunkIndex].blockTypes[x][y][z];
				uint explosionValue = chunksData[chunkIndex].explosions[x][y][z];
				if(blockType != 0 && explosionValue == 1){
					ivec3 blockLocalPosition = ivec3(x,y,z);
					uint blockLocalPositionBinary = blockLocalPosition.x | blockLocalPosition.y<<4 | blockLocalPosition.z <<8;
					
					for(int n = 0;n<6;n++){
						ivec3 neighbourPos = blockLocalPosition + offsets[n];
						ivec3 neighbourChunkOffset = ivec3(0);

						// Handle boundaries
						if (neighbourPos.x < 0) { neighbourPos.x = CHUNK_WIDTH - 1; neighbourChunkOffset.x = -1; }
						else if (neighbourPos.x >= CHUNK_WIDTH) { neighbourPos.x = 0; neighbourChunkOffset.x = 1; }
						if (neighbourPos.y < 0) { neighbourPos.y = CHUNK_WIDTH - 1; neighbourChunkOffset.y = -1; }
						else if (neighbourPos.y >= CHUNK_WIDTH) { neighbourPos.y = 0; neighbourChunkOffset.y = 1; }
						if (neighbourPos.z < 0) { neighbourPos.z = CHUNK_WIDTH - 1; neighbourChunkOffset.z = -1; }
						else if (neighbourPos.z >= CHUNK_WIDTH) { neighbourPos.z = 0; neighbourChunkOffset.z = 1; }

						uint neighbourType = 1;
						uvec3 neighbourChunkPosition = uvec3( gl_WorkGroupID.x + neighbourChunkOffset.x,
															gl_WorkGroupID.y + neighbourChunkOffset.y,
															gl_WorkGroupID.z + neighbourChunkOffset.z
															);	
						if(!(neighbourChunkPosition.x>=WORLD_WIDTH ||neighbourChunkPosition.y>=WORLD_HEIGHT ||neighbourChunkPosition.z>=WORLD_WIDTH ||
						neighbourChunkPosition.x<0 ||neighbourChunkPosition.y<0 ||neighbourChunkPosition.z<0)){
							uint neighbourChunkIndex = neighbourChunkPosition.x
												 + neighbourChunkPosition.y * WORLD_WIDTH
												 + neighbourChunkPosition.z * WORLD_WIDTH * WORLD_HEIGHT;
							neighbourType = chunksData[neighbourChunkIndex]
								.blockTypes[neighbourPos.x][neighbourPos.y][neighbourPos.z];
						}
						if (neighbourType == 0) {
							chunksQuads[chunkIndex].blockQuads[index] = blockLocalPositionBinary | n << 12 | blockTypeAndNormalToTextureId(blockType, n) << 15;
							index++;
						}
					}
				}

			}
		}
	}
	renderData[chunkIndex] = index;

}
