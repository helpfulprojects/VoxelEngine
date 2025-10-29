#type compute

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};
struct Node{
uint localPosition;
uint chunkPosition;
uint previousValue;
};
struct Queue{
	Node nodes[300];
};
layout(std430, binding = 7) buffer buffer7
{
	Queue chunksQueue[]; 
};
layout(std430, binding = 8) buffer buffer8
{
	bool shouldRedrawWorld; 
};

layout(std430, binding = 9) buffer buffer9
{
	bool shouldRedrawChunk[]; 
};

//uniform uvec3 u_Offset;

void propagateExplosion(uint chunkIndex, int x, int y, int z){
	Queue queue = chunksQueue[chunkIndex];
	uint blockLocalPositionBinary = x | y<<4 | z <<8;
	int front = 0;
	int back = 0;
	queue.nodes[back++] = Node(blockLocalPositionBinary,0,1);
	while(front<back){
		Node node = queue.nodes[front++];
		const uint x = (node.localPosition) & MASK_4_BITS;
		const uint y = (node.localPosition >> 4) & MASK_4_BITS;
		const uint z = (node.localPosition >> 8) & MASK_4_BITS;
		chunksData[chunkIndex].blockTypes[x][y][z] = 0;
		if(node.previousValue == 1){
			queue.nodes[back++] = Node(x+1 | y<<4 | z <<8,0,0);
		}
	}
}

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
//	uint chunkX = (gl_WorkGroupID.x+u_Offset.x)*2;
//	uint chunkY = (gl_WorkGroupID.y+u_Offset.y)*2;
//	uint chunkZ = (gl_WorkGroupID.z+u_Offset.z)*2;
	uint chunkX = 1;
	uint chunkY = 0;
	uint chunkZ = 1;
	uint chunkIndex = chunkX+chunkY*WORLD_WIDTH+chunkZ*WORLD_WIDTH*WORLD_HEIGHT;
	if(!chunksData[chunkIndex].hasExplosion){
		return;
	}

	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int z=0;z<CHUNK_WIDTH;z++){
			for(int y=0;y<CHUNK_WIDTH;y++){
				//maybe have to deal with unpacking here and packing later on??
				uint explosionValue = chunksData[chunkIndex].explosions[x][y][z];
				//uint explosionValue = chunksData[chunkIndex].explosions[x][y][z] & MASK_3_BITS;
				//uint explosionOwner = (chunksData[chunkIndex].explosions[x][y][z]>>3) & MASK_24_BITS;
				if(explosionValue==TNT_EXPLOSION_STRENGTH){
					chunksData[chunkIndex].explosions[x][y][z] = 0;
					propagateExplosion(chunkIndex,x,y,z);
					//chunksData[chunkIndex].blockTypes[x][y][z] = 0;
					shouldRedrawWorld = true;
					shouldRedrawChunk[chunkIndex] = true;
					//dfs(Node(x,y,z,chunkX,chunkY,chunkZ,explosionValue+1,explosionOwner));
				}
				//find explosions, start bfs from each one, can go outside of the chunk
				//chunksData[chunkIndex].explosions[x][y][z] = 0;
			}
		}
	}
	chunksData[chunkIndex].hasExplosion = false;
}
