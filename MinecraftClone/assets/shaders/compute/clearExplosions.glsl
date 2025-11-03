#type compute

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
	uint chunkIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*WORLD_WIDTH+gl_WorkGroupID.z*WORLD_WIDTH*WORLD_HEIGHT;
	if(!chunksData[chunkIndex].hasExplosion){
		return;
	}
//	for(int x=0;x<CHUNK_WIDTH;x++){
//		for(int z=0;z<CHUNK_WIDTH;z++){
//			for(int y=0;y<CHUNK_WIDTH;y++){
//				if(chunksData[chunkIndex].explosions[x][y][z]!=1){
//					chunksData[chunkIndex].explosions[x][y][z] = 0;
//				}
//			}
//		}
//	}
	chunksData[chunkIndex].hasExplosion = false;
}
