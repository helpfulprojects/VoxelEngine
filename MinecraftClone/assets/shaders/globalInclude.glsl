#define DEFAULT_SPAWN vec3(CHUNK_SIDE_LENGTH * WORLD_WIDTH / 2, CHUNK_SIDE_LENGTH * WORLD_HEIGHT*2, CHUNK_SIDE_LENGTH * WORLD_WIDTH / 2)
#define GRAVITY -28.57
#define TNT_EXPLOSION_STRENGTH 4

#define air 0
#define dirt_block 1
#define grass_block 2
#define tnt_block 3
#define bedrock_block 4
#define stone_block 5

#define top 0
#define bottom 1
#define east 2
#define west 3
#define south 4
#define north 5


#define dirt 0
#define grass_block_top 1
#define grass_block_side 2
#define stone 3
#define tnt_bottom 4
#define tnt_side 5
#define tnt_top 6
#define bedrock 7

uint getChunkIndex(uint chunkX, uint chunkY, uint chunkZ){
	return chunkX+chunkY*WORLD_WIDTH+chunkZ*WORLD_WIDTH*WORLD_HEIGHT;
}

struct Chunk {
	uint x;
	uint y;
	uint z;
	bool hasExplosion;
	uint blockTypes[CHUNK_SIDE_LENGTH][CHUNK_SIDE_LENGTH][CHUNK_SIDE_LENGTH];
	uint explosions[CHUNK_SIDE_LENGTH][CHUNK_SIDE_LENGTH][CHUNK_SIDE_LENGTH];
};

struct ChunkQuads {
	uint blockQuads[FACES_PER_CHUNK];
};

struct TntEntity{
	vec3 position;
	vec3 velocity;
	float secondsUntilExplode;
	bool visible; 
	bool justBlewUp; 
};