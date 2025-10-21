#type compute
#version 430 core

#define CHUNK_WIDTH 16
#define WORLD_WIDTH 65
#define WORLD_HEIGHT 16

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Chunk {
	uint x;
	uint y;
	uint z;
	int quadsCount;
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint blockQuads[CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH/2*6];
};

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
	int index = 0;
	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int y=0;y<CHUNK_WIDTH;y++){
			for(int z=0;z<CHUNK_WIDTH;z++){
				chunksData[chunkIndex].x = gl_WorkGroupID.x * CHUNK_WIDTH;
				chunksData[chunkIndex].y = gl_WorkGroupID.y * CHUNK_WIDTH;
				chunksData[chunkIndex].z = gl_WorkGroupID.z * CHUNK_WIDTH;
				if(y==15&&gl_WorkGroupID.y==15){
					chunksData[chunkIndex].blockTypes[x][y][z] = 2;
				}else{
					chunksData[chunkIndex].blockTypes[x][y][z] = 1;
				}
				chunksData[chunkIndex].quadsCount = 0;
			}
		}
	}
}
