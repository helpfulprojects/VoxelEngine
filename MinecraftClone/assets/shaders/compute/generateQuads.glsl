#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 1
#define WORKGROUP_SIZE_X 1

layout (local_size_x = WORKGROUP_SIZE_X, local_size_y = WORKGROUP_SIZE_X, local_size_z = WORKGROUP_SIZE_X) in;

struct Chunk {
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint blockQuads[CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH*6];
};

layout(std430, binding = 0) readonly buffer buffer0 
{
	Chunk chunksData[WORLD_WIDTH][WORLD_WIDTH][WORLD_WIDTH]; 
};

layout(std430, binding = 1) buffer buffer1 
{
	ChunkQuads chunksQuads[WORLD_WIDTH][WORLD_WIDTH][WORLD_WIDTH]; 
};

void main() {
	uvec3 chunkId = gl_WorkGroupID;
//	ivec3 chunkPos = chunkId*CHUNK_WIDTH;
//See how to get a reference
	Chunk chunkData = chunksData[0][0][0];
	//ChunkQuads chunkQuads = chunksQuads[chunkId.x][chunkId.y][chunkId.z];
	uint index = 0;
	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int y=0;y<CHUNK_WIDTH;y++){
			for(int z=0;z<CHUNK_WIDTH;z++){
				if(chunkData.blockTypes[x][y][z] == 1){
					ivec3 blockLocalPosition = ivec3(x,y,z);
					uint blockLocalPositionBinary = blockLocalPosition.x | blockLocalPosition.y<<4 | blockLocalPosition.z <<8;
					for(int i = 0;i<6;i++){
						chunksQuads[0][0][0].blockQuads[index] = blockLocalPositionBinary | i << 16 | 0 << 19 ;
						index++;
					}
				}
			}
		}
	}

}
