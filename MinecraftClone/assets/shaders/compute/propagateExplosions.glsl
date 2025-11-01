#type compute

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};
struct Node{
uint localIndex3D;
uint chunkIndex3D;
uint chunkIndex;
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

uniform vec3 u_Offset;

ivec3 offsets[6] = ivec3[6](
    ivec3(0, 1, 0),
    ivec3(0, -1, 0),
    ivec3(1, 0, 0),
    ivec3(-1, 0, 0),
    ivec3(0, 0, 1),
    ivec3(0, 0, -1)
);
uint startingChunkX = (gl_WorkGroupID.x+uint(u_Offset.x))*1;
uint startingChunkY = (gl_WorkGroupID.y+uint(u_Offset.y))*1;
uint startingChunkZ = (gl_WorkGroupID.z+uint(u_Offset.z))*1;
void propagateExplosion(uint chunkIndex, int x, int y, int z){
	shouldRedrawWorld = true;
	Queue queue = chunksQueue[chunkIndex];
	int front = 0;
	int back = 0;
	queue.nodes[back++] = Node(x | y<<4 | z <<8,startingChunkX | startingChunkY<<7 | startingChunkZ <<11,chunkIndex,TNT_EXPLOSION_STRENGTH+1);
	while(front<back){
		Node node = queue.nodes[front++];

		uint x = (node.localIndex3D) & MASK_4_BITS;
		uint y = (node.localIndex3D >> 4) & MASK_4_BITS;
		uint z = (node.localIndex3D >> 8) & MASK_4_BITS;

		uint chunkX = (node.chunkIndex3D) & MASK_7_BITS;
		uint chunkY = (node.chunkIndex3D >> 7) & MASK_4_BITS;
		uint chunkZ = (node.chunkIndex3D >> 11) & MASK_7_BITS;

		uint chunkIndex = node.chunkIndex;

		chunksData[chunkIndex].blockTypes[x][y][z] = 0;
		shouldRedrawChunk[chunkIndex] = true;
		uint explosionValue = (node.previousValue & MASK_3_BITS)-1;
		chunksData[chunkIndex].explosions[x][y][z] = explosionValue;

		ivec3 blockLocalPosition = ivec3(x,y,z);

		if(explosionValue>1){
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

				uvec3 neighbourChunk3DIndex = uvec3( chunkX + neighbourChunkOffset.x,
													chunkY + neighbourChunkOffset.y,
													chunkZ + neighbourChunkOffset.z
													);	

				if(!(neighbourChunk3DIndex.x>=WORLD_WIDTH ||neighbourChunk3DIndex.y>=WORLD_HEIGHT ||neighbourChunk3DIndex.z>=WORLD_WIDTH ||
				neighbourChunk3DIndex.x<0 ||neighbourChunk3DIndex.y<0 ||neighbourChunk3DIndex.z<0)){
					uint neighbourChunkIndex = neighbourChunk3DIndex.x
										 + neighbourChunk3DIndex.y * WORLD_WIDTH
										 + neighbourChunk3DIndex.z * WORLD_WIDTH * WORLD_HEIGHT;
					if(chunksData[neighbourChunkIndex].explosions[neighbourPos.x][neighbourPos.y][neighbourPos.z] < explosionValue-1){
						queue.nodes[back++] = Node(neighbourPos.x | neighbourPos.y<<4 | neighbourPos.z <<8,neighbourChunk3DIndex.x | neighbourChunk3DIndex.y<<7 | neighbourChunk3DIndex.z <<11,neighbourChunkIndex,explosionValue);
					}
	//				neighbourType = chunksData[neighbourChunkIndex]
	//					.blockTypes[neighbourPos.x][neighbourPos.y][neighbourPos.z];
				}
			}
		}
	}
}

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
	uint chunkIndex = startingChunkX+startingChunkY*WORLD_WIDTH+startingChunkZ*WORLD_WIDTH*WORLD_HEIGHT;
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
					propagateExplosion(chunkIndex,x,y,z);
					//chunksData[chunkIndex].blockTypes[x][y][z] = 0;
					//dfs(Node(x,y,z,chunkX,chunkY,chunkZ,explosionValue+1,explosionOwner));
				}
				//find explosions, start bfs from each one, can go outside of the chunk
				//chunksData[chunkIndex].explosions[x][y][z] = 0;
			}
		}
	}
}
