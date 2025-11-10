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
	Node nodes[63];
};
layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
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
uint startingChunkX = (gl_WorkGroupID.x)*2+uint(u_Offset.x);
uint startingChunkY = (gl_WorkGroupID.y)*2+uint(u_Offset.y);
uint startingChunkZ = (gl_WorkGroupID.z)*2+uint(u_Offset.z);
void propagateExplosion(uint chunkIndex, int x, int y, int z){
	shouldRedrawWorld = true;
	uint queueIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*HALF_WORLD_WIDTH+gl_WorkGroupID.z*HALF_WORLD_WIDTH*HALF_WORLD_HEIGHT;
	Queue queue = chunksQueue[queueIndex];
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
		shouldRedrawChunk[chunkIndex] = true;
		// If block is on the edge, flag the neighboring chunk(s)
		if (x == 0 && chunkX > 0)
			shouldRedrawChunk[getChunkIndex(chunkX - 1, chunkY, chunkZ)] = true;
		else if (x == CHUNK_WIDTH - 1)
			shouldRedrawChunk[getChunkIndex(chunkX + 1, chunkY, chunkZ)] = true;

		if (y == 0 && chunkY > 0)
			shouldRedrawChunk[getChunkIndex(chunkX, chunkY - 1, chunkZ)] = true;
		else if (y == CHUNK_WIDTH - 1)
			shouldRedrawChunk[getChunkIndex(chunkX, chunkY + 1, chunkZ)] = true;

		if (z == 0 && chunkZ > 0)
			shouldRedrawChunk[getChunkIndex(chunkX, chunkY, chunkZ - 1)] = true;
		else if (z == CHUNK_WIDTH - 1)
			shouldRedrawChunk[getChunkIndex(chunkX, chunkY, chunkZ + 1)] = true;

		int blockType = int(chunksData[chunkIndex].blockTypes[x][y][z]);
		if(blockType == tnt_block){
			vec3 blockOrigin = vec3(chunkX*CHUNK_WIDTH+x,chunkY*CHUNK_WIDTH+y,chunkZ*CHUNK_WIDTH+z);	
			int relX = int(blockOrigin.x - DEFAULT_SPAWN.x);
			int relY = int(blockOrigin.y - 101);
			int relZ = int(blockOrigin.z - DEFAULT_SPAWN.z);

			int tntIndex = relY * TNT_WIDTH * TNT_WIDTH + relZ * TNT_WIDTH + relX;
			tnts[tntIndex].position = blockOrigin;
			tnts[tntIndex].velocity = vec3(0,1,0);
			tnts[tntIndex].visible = true;
			vec3 rand = hash33(blockOrigin);
			tnts[tntIndex].secondsUntilExplode = 0.5+fract(rand.x+rand.y+rand.z);
		}
		if(blockType!=bedrock_block){
			chunksData[chunkIndex].blockTypes[x][y][z] = 0;
		}
		uint explosionValue = node.previousValue -1;
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
						chunksData[neighbourChunkIndex].explosions[neighbourPos.x][neighbourPos.y][neighbourPos.z] = explosionValue-1;
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
				uint explosionValue = chunksData[chunkIndex].explosions[x][y][z];
				if(explosionValue==TNT_EXPLOSION_STRENGTH){
					propagateExplosion(chunkIndex,x,y,z);
				}
			}
		}
	}
}
