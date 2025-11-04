#type vertex

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
const vec3 offset = vec3(0.5, 0.5, 0.5);
const vec3 facePositions[6][4] = vec3[6][4](
    // +Y (top)
    vec3[4](
        vec3( 0.5,  0.5, -0.5) + offset,
        vec3(-0.5,  0.5, -0.5) + offset,
        vec3(-0.5,  0.5,  0.5) + offset,
        vec3( 0.5,  0.5,  0.5) + offset
    ),
    // -Y (bottom)
    vec3[4](
        vec3( 0.5, -0.5,  0.5) + offset,
        vec3(-0.5, -0.5,  0.5) + offset,
        vec3(-0.5, -0.5, -0.5) + offset,
        vec3( 0.5, -0.5, -0.5) + offset
    ),
    // +X (east)
    vec3[4](
        vec3( 0.5, -0.5,  0.5) + offset,
        vec3( 0.5, -0.5, -0.5) + offset,
        vec3( 0.5,  0.5, -0.5) + offset,
        vec3( 0.5,  0.5,  0.5) + offset
    ),
    // -X (west)
    vec3[4](
        vec3(-0.5, -0.5, -0.5) + offset,
        vec3(-0.5, -0.5,  0.5) + offset,
        vec3(-0.5,  0.5,  0.5) + offset,
        vec3(-0.5,  0.5, -0.5) + offset
    ),
    // +Z (south)
    vec3[4](
        vec3(-0.5, -0.5,  0.5) + offset,
        vec3( 0.5, -0.5,  0.5) + offset,
        vec3( 0.5,  0.5,  0.5) + offset,
        vec3(-0.5,  0.5,  0.5) + offset
    ),
    // -Z (north)
    vec3[4](
        vec3( 0.5, -0.5, -0.5) + offset,
        vec3(-0.5, -0.5, -0.5) + offset,
        vec3(-0.5,  0.5, -0.5) + offset,
        vec3( 0.5,  0.5, -0.5) + offset
    )
);

int indices[6] = {0, 1, 2, 2, 3, 0};

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec4 v_StaticLight;
void main()
{
	const int index = gl_VertexID/6;

    const uint chunkId = gl_BaseInstance;
    uint chunkX = chunksData[chunkId].x;
    uint chunkY = chunksData[chunkId].y;
    uint chunkZ = chunksData[chunkId].z;

	const uint packedData = chunksQuads[chunkId].blockQuads[index];
	const int currVertexID = gl_VertexID % 6;

	const uint x = (packedData) & MASK_4_BITS;
	const uint y = (packedData >> 4) & MASK_4_BITS;
	const uint z = (packedData >> 8) & MASK_4_BITS;

	const uint normalId = (packedData >> 12) & MASK_3_BITS;
	const uint texId = (packedData >> 15) & MASK_4_BITS;
	
	vec3 position = vec3(x+chunkX, y+chunkY, z+chunkZ);

	position += facePositions[normalId][indices[currVertexID]];

    v_TexCoord = terrainAtlasCoords[texId]+texturePositionOffsets[indices[currVertexID]];
    v_StaticLight = vec4(1.0,1.0,1.0,1.0);
    if(normalId == east || normalId == west){
		v_StaticLight = vec4(0.8,0.8,0.8,1.0);
    }else if(normalId == south || normalId == north){
		v_StaticLight = vec4(0.7,0.7,0.7,1.0);
    }else{
		v_StaticLight = vec4(1.0,1.0,1.0,1.0);
    }
    
	gl_Position = u_ViewProjection*u_Transform*vec4(position, 1.0);
}




















#type fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoord;
in vec4 v_StaticLight;

uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture,v_TexCoord)*v_StaticLight;
	//color = vec4(1.0f,0.0f,0.0f,1.0f);
}
