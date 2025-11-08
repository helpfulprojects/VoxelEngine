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

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
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
out vec4 v_ColorOverlay;
void main()
{
	const int index = gl_VertexID/6;

	const int currVertexID = gl_VertexID % 6;
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
	
    bool showldDraw = tnts[gl_InstanceID].visible;
    //bool showldDraw = true;
    if(showldDraw){
		vec3 position = tnts[gl_InstanceID].position;

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
    }else{
		v_StaticLight = vec4(0.0,0.0,0.0,0.0);
		gl_Position = vec4(0,0,0,0);
    }

	v_ColorOverlay = vec4(1,1,1,1);
	if((int(tnts[gl_InstanceID].secondsUntilExplode*5)&1)==0){
		v_ColorOverlay = vec4(1,1,1,0.247);
	}
}




















#type fragment
#version 330 core

out vec4 color;
in vec2 v_TexCoord;
in vec4 v_StaticLight;
in vec4 v_ColorOverlay;

uniform sampler2D u_Texture;

void main()
{
    if(v_StaticLight.a==0){
        discard;
    }
	color = texture(u_Texture,v_TexCoord)*v_StaticLight;
	color.rgb = mix(v_ColorOverlay.rgb,color.rgb,v_ColorOverlay.a);
}
