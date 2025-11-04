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
					chunksQuads[chunkIndex].blockQuads[index] = (x | y<<4 | z <<8);
					index++;
				}

			}
		}
	}
	renderData[chunkIndex] = index;

}
