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

ivec3 offsets[6] = ivec3[6](
    ivec3(0, 1, 0),
    ivec3(0, -1, 0),
    ivec3(1, 0, 0),
    ivec3(-1, 0, 0),
    ivec3(0, 0, 1),
    ivec3(0, 0, -1)
);

int blockTypeAndNormalToTextureId(uint blockType, int normal){
	switch(blockType){
	case 0://AIR
		return dirt;
	case 1://DIRT
		return dirt;
	case 2://GRASS_BLOCK
		switch(normal){
		case 0://positive y
			return grass_block_top;
		case 1://negative y
			return dirt;
		case 2://positive x
			return grass_block_side;
		case 3://negative x
			return grass_block_side;
		case 4://positive z
			return grass_block_side;
		case 5://negative z
			return grass_block_side;
		}
	}
}

void main() {
	uint chunkIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*WORLD_WIDTH+gl_WorkGroupID.z*WORLD_WIDTH*WORLD_HEIGHT;
	int index = 0;
	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int y=0;y<CHUNK_WIDTH;y++){
			for(int z=0;z<CHUNK_WIDTH;z++){
				uint blockType = chunksData[chunkIndex].blockTypes[x][y][z];
				if(blockType != 0){
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

//					for(int n = 0;n<6;n++){
//						chunksQuads[chunkIndex].blockQuads[index] = blockLocalPositionBinary | n << 16 | 0 << 19 ;
//						index++;
//					}
				}

			}
		}
	}
	renderData[chunkIndex] = index;

}
