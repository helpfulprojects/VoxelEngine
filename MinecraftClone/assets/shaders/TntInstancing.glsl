#type vertex
#version 460 core
#define CHUNK_WIDTH 16
#define BLOCKS_IN_CHUNK_COUNT CHUNK_WIDTH*CHUNK_WIDTH*CHUNK_WIDTH
#define FACES_PER_CHUNK BLOCKS_IN_CHUNK_COUNT

struct Chunk {
	int x;
	int y;
	int z;
	uint blockTypes[CHUNK_WIDTH][CHUNK_WIDTH][CHUNK_WIDTH];
};

struct ChunkQuads {
	uint blockQuads[FACES_PER_CHUNK];
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
out vec4 v_StaticLight;
#define top 0
#define bottom 1
#define east 2
#define west 3
#define south 4
#define north 5

#define tnt_bottom 4
#define tnt_side 5
#define tnt_top 6
void main()
{
	const uint MASK_3_BITS = (1u << 3) - 1u;
	const uint MASK_4_BITS = (1u << 4) - 1u;
	const uint MASK_8_BITS = (1u << 8) - 1u;
	const int index = gl_VertexID/6;

	const int currVertexID = gl_VertexID % 6;

	const uint x = 0;
	const uint y = 0;
	const uint z = 0;
    
    const int size = 100;
    int xOffset = gl_InstanceID%size;
    int yOffset = (gl_InstanceID/size)%size;
    int zOffset = gl_InstanceID/(size*size);
     
	const uint normalId = index;
	uint texId = 0;
    switch(normalId){
		case 0://positive y
			texId = tnt_top;
            break;
		case 1://negative y
			texId = tnt_bottom;
            break;
		case 2://positive x
			texId = tnt_side;
            break;
		case 3://negative x
			texId = tnt_side;
            break;
		case 4://positive z
			texId = tnt_side;
            break;
		case 5://negative z
			texId = tnt_side;
            break;
    }
	
	vec3 position = vec3(x+xOffset, y+yOffset, z+zOffset);

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
}
