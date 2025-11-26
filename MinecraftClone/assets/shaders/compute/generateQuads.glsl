#type compute
#version 430 core
#includeGlobalSrouce

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;


layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};

layout(std430, binding = 1) buffer buffer1 
{
	ChunkQuads chunksQuads[]; 
};
layout(std430, binding = 5) buffer buffer5
{
	uint renderData[];
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

int blockTypeAndNormalToTextureId(uint blockType, int normal){
	switch(blockType){
	case air:
		return dirt;
	case dirt_block:
		return dirt;
	case grass_block:
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
	case tnt_block:
		switch(normal){
		case 0://positive y
			return tnt_top;
		case 1://negative y
			return tnt_bottom;
		case 2://positive x
			return tnt_side;
		case 3://negative x
			return tnt_side;
		case 4://positive z
			return tnt_side;
		case 5://negative z
			return tnt_side;
		}
	case bedrock_block:
		return bedrock;
	case stone_block:
		return stone;
	}
}

void main() {
	uint chunkIndex = getChunkIndex(gl_WorkGroupID.x,gl_WorkGroupID.y,gl_WorkGroupID.z);
	if(!shouldRedrawChunk[chunkIndex]){
		return;	
	}
	int index = 0;
	for(int x=0;x<CHUNK_SIDE_LENGTH;x++){
		for(int y=0;y<CHUNK_SIDE_LENGTH;y++){
			for(int z=0;z<CHUNK_SIDE_LENGTH;z++){
				uint blockType = chunksData[chunkIndex].blockTypes[x][y][z];
				if(blockType != 0){
					ivec3 blockLocalPosition = ivec3(x,y,z);
					uint blockLocalPositionBinary = blockLocalPosition.x | blockLocalPosition.y<<4 | blockLocalPosition.z <<8;
					for(int n = 0;n<6;n++){
						ivec3 neighbourPos = blockLocalPosition + offsets[n];
						ivec3 neighbourChunkOffset = ivec3(0);

						// Handle boundaries
						if (neighbourPos.x < 0) { neighbourPos.x = CHUNK_SIDE_LENGTH - 1; neighbourChunkOffset.x = -1; }
						else if (neighbourPos.x >= CHUNK_SIDE_LENGTH) { neighbourPos.x = 0; neighbourChunkOffset.x = 1; }
						if (neighbourPos.y < 0) { neighbourPos.y = CHUNK_SIDE_LENGTH - 1; neighbourChunkOffset.y = -1; }
						else if (neighbourPos.y >= CHUNK_SIDE_LENGTH) { neighbourPos.y = 0; neighbourChunkOffset.y = 1; }
						if (neighbourPos.z < 0) { neighbourPos.z = CHUNK_SIDE_LENGTH - 1; neighbourChunkOffset.z = -1; }
						else if (neighbourPos.z >= CHUNK_SIDE_LENGTH) { neighbourPos.z = 0; neighbourChunkOffset.z = 1; }

						uint neighbourType = 1;
						uvec3 neighbourChunkPosition = uvec3( gl_WorkGroupID.x + neighbourChunkOffset.x,
															gl_WorkGroupID.y + neighbourChunkOffset.y,
															gl_WorkGroupID.z + neighbourChunkOffset.z
															);	
						if(!(neighbourChunkPosition.x>=WORLD_WIDTH ||neighbourChunkPosition.y>=WORLD_HEIGHT ||neighbourChunkPosition.z>=WORLD_WIDTH ||
						neighbourChunkPosition.x<0 ||neighbourChunkPosition.y<0 ||neighbourChunkPosition.z<0)){
							uint neighbourChunkIndex = getChunkIndex(neighbourChunkPosition.x,neighbourChunkPosition.y,neighbourChunkPosition.z);
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
