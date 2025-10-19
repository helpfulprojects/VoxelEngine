#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 2

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Chunk {
	int x;
	int y;
	int z;
	int quadsCount;
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint blockQuads[CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH*6];
};

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[WORLD_WIDTH*WORLD_WIDTH*WORLD_WIDTH]; 
};

layout(std430, binding = 1) buffer buffer1 
{
	ChunkQuads chunksQuads[WORLD_WIDTH*WORLD_WIDTH*WORLD_WIDTH]; 
};
layout(std430, binding = 4) buffer buffer4
{
	uvec3 debugBuffer[];
};

ivec3 offsets[6] = ivec3[6](
    ivec3(-1, 0, 0),
    ivec3(1, 0, 0),
    ivec3(0, -1, 0),
    ivec3(0, 1, 0),
    ivec3(0, 0, -1),
    ivec3(0, 0, 1)
);

void main() {
	uint chunkIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*2+gl_WorkGroupID.z*2*2;
	int index = 0;
	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int y=0;y<CHUNK_WIDTH;y++){
			for(int z=0;z<CHUNK_WIDTH;z++){

				if(chunksData[chunkIndex].blockTypes[x][y][z] == 1){
					ivec3 blockLocalPosition = ivec3(x,y,z);
					uint blockLocalPositionBinary = blockLocalPosition.x | blockLocalPosition.y<<4 | blockLocalPosition.z <<8;
//					for(int n = 0;n<6;n++){
//						ivec3 neighbourPos = blockLocalPosition + offsets[n];
//						ivec3 neighbourChunkOffset = ivec3(0);
//
//						// Handle boundaries
//						if (neighbourPos.x < 0) { neighbourPos.x = CHUNK_WIDTH - 1; neighbourChunkOffset.x = -1; }
//						else if (neighbourPos.x >= CHUNK_WIDTH) { neighbourPos.x = 0; neighbourChunkOffset.x = 1; }
//						if (neighbourPos.y < 0) { neighbourPos.y = CHUNK_WIDTH - 1; neighbourChunkOffset.y = -1; }
//						else if (neighbourPos.y >= CHUNK_WIDTH) { neighbourPos.y = 0; neighbourChunkOffset.y = 1; }
//						if (neighbourPos.z < 0) { neighbourPos.z = CHUNK_WIDTH - 1; neighbourChunkOffset.z = -1; }
//						else if (neighbourPos.z >= CHUNK_WIDTH) { neighbourPos.z = 0; neighbourChunkOffset.z = 1; }
//
//						uint neighbourChunkIndex = (gl_WorkGroupID.x + neighbourChunkOffset.x)
//												 + (gl_WorkGroupID.y + neighbourChunkOffset.y) * WORLD_WIDTH
//												 + (gl_WorkGroupID.z + neighbourChunkOffset.z) * WORLD_WIDTH * WORLD_WIDTH;
//						uint neighbourType = 0;
//						if(neighbourChunkIndex >= 0 && neighbourChunkIndex < WORLD_WIDTH*WORLD_WIDTH*WORLD_WIDTH){
//							neighbourType = chunksData[neighbourChunkIndex]
//								.blockTypes[neighbourPos.x][neighbourPos.y][neighbourPos.z];
//						}
//						if (neighbourType == 0) {
//							chunksQuads[chunkIndex].blockQuads[index] = blockLocalPositionBinary | n << 16 | 0 << 19;
//							index++;
//						}
//						//chunksQuads[chunkIndex].blockQuads[index] = blockLocalPositionBinary | i << 16 | 0 << 19 ;
//						//index++;
//					}
					for(int n = 0;n<6;n++){
						chunksQuads[chunkIndex].blockQuads[index] = blockLocalPositionBinary | n << 16 | 0 << 19 ;
						index++;
					}
				}

			}
		}
	}
	chunksData[chunkIndex].quadsCount = index;

}
