#type vertex
#version 460 core
#define CHUNK_WIDTH 16

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

layout(std430, binding = 0) readonly buffer buffer0 
{
	Chunk chunksData[]; 
};
layout(std430, binding = 1) readonly buffer buffer1 
{
	ChunkQuads chunksQuads[]; 
};

layout(std430, binding = 2) readonly buffer terrainAtlasCoordsBuffer
{
	vec2 terrainAtlasCoords[]; 
};
layout(std430, binding = 3) readonly buffer texturePositionOffsetsBuffer
{
	vec2 texturePositionOffsets[]; 
};

layout(std430, binding = 4) readonly buffer buffer4
{
	uvec3 debugBuffer[];
};
const vec3 facePositions[6][4] = vec3[6][4](
    // +Y (top)
    vec3[4](
        vec3( 0.5,  0.5, -0.5),
        vec3(-0.5,  0.5, -0.5),
        vec3(-0.5,  0.5,  0.5),
        vec3( 0.5,  0.5,  0.5)
    ),
    // -Y (bottom)
    vec3[4](
        vec3( 0.5, -0.5,  0.5),
        vec3(-0.5, -0.5,  0.5),
        vec3(-0.5, -0.5, -0.5),
        vec3( 0.5, -0.5, -0.5)
    ),
    // +X (east)
    vec3[4](
        vec3( 0.5, -0.5,  0.5),
        vec3( 0.5, -0.5, -0.5),
        vec3( 0.5,  0.5, -0.5),
        vec3( 0.5,  0.5,  0.5)
    ),
    // -X (west)
    vec3[4](
        vec3(-0.5, -0.5, -0.5),
        vec3(-0.5, -0.5,  0.5),
        vec3(-0.5,  0.5,  0.5),
        vec3(-0.5,  0.5, -0.5)
    ),
    // +Z (south)
    vec3[4](
        vec3(-0.5, -0.5,  0.5),
        vec3( 0.5, -0.5,  0.5),
        vec3( 0.5,  0.5,  0.5),
        vec3(-0.5,  0.5,  0.5)
    ),
    // -Z (north)
    vec3[4](
        vec3( 0.5, -0.5, -0.5),
        vec3(-0.5, -0.5, -0.5),
        vec3(-0.5,  0.5, -0.5),
        vec3( 0.5,  0.5, -0.5)
    )
);

int indices[6] = {0, 1, 2, 2, 3, 0};

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main()
{
	const uint MASK_3_BITS = (1u << 3) - 1u;
	const uint MASK_4_BITS = (1u << 4) - 1u;
	const uint MASK_8_BITS = (1u << 8) - 1u;
	const int index = gl_VertexID/6;

    const uint chunkId = gl_BaseInstance;
    int chunkX = chunksData[chunkId].x;
    int chunkY = chunksData[chunkId].y;
    int chunkZ = chunksData[chunkId].z;

	const uint packedData = chunksQuads[chunkId].blockQuads[index];
	const int currVertexID = gl_VertexID % 6;

	const uint x = (packedData) & MASK_4_BITS;
	const uint y = (packedData >> 4) & MASK_4_BITS;
	const uint z = (packedData >> 8) & MASK_8_BITS;

	const uint normalId = (packedData >> 16) & MASK_3_BITS;
	const uint texId = (packedData >> 19) & MASK_3_BITS;
	
	vec3 position = vec3(x+chunkX, y+chunkY, z+chunkZ);

	position += facePositions[normalId][indices[currVertexID]];

    v_TexCoord = terrainAtlasCoords[texId]+texturePositionOffsets[indices[currVertexID]];
    
	gl_Position = u_ViewProjection*u_Transform*vec4(position, 1.0);
}




















#type fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture,v_TexCoord);
	//color = vec4(1.0f,0.0f,0.0f,1.0f);
}
