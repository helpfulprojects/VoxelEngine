#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 2

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Chunk {
	int x;
	int y;
	int z;
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint blockQuads[CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH*6];
};

layout(std430, binding = 0) readonly buffer buffer0 
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

void main() {
	//uint chunkIndex = gl_LocalInvocationIndex;
	uint chunkIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*2+gl_WorkGroupID.z*2*2;
//	ivec3 chunkPos = chunkId*CHUNK_WIDTH;
//See how to get a reference
	//Chunk chunkData = chunksData[chunkId.x][chunkId.y][chunkId.z];
	//ChunkQuads chunkQuads = chunksQuads[chunkId.x][chunkId.y][chunkId.z];
	uint index = 0;
	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int y=0;y<CHUNK_WIDTH;y++){
			for(int z=0;z<CHUNK_WIDTH;z++){
				if(chunksData[chunkIndex].blockTypes[x][y][z] == 1){
					ivec3 blockLocalPosition = ivec3(x,y,z);
					uint blockLocalPositionBinary = blockLocalPosition.x | blockLocalPosition.y<<4 | blockLocalPosition.z <<8;
					for(int i = 0;i<6;i++){
						chunksQuads[chunkIndex].blockQuads[index] = blockLocalPositionBinary | i << 16 | 0 << 19 ;
						index++;
					}
				}
			}
		}
	}

}
