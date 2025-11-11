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

out vData{
	vec2 v_TexCoord;
	vec4 v_StaticLight;
	vec4 v_ColorOverlay;
	bool v_PassThrough;
} vertex;
#line 0
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
		vertex.v_PassThrough = true;
		vec3 position = tnts[gl_InstanceID].position;

		position += facePositions[normalId][indices[currVertexID]];

		vertex.v_TexCoord = terrainAtlasCoords[texId]+texturePositionOffsets[indices[currVertexID]];
		vertex.v_StaticLight = vec4(1.0,1.0,1.0,1.0);
		if(normalId == east || normalId == west){
			vertex.v_StaticLight = vec4(0.8,0.8,0.8,1.0);
		}else if(normalId == south || normalId == north){
			vertex.v_StaticLight = vec4(0.7,0.7,0.7,1.0);
		}else{
			vertex.v_StaticLight = vec4(1.0,1.0,1.0,1.0);
		}
		
		gl_Position = u_ViewProjection*u_Transform*vec4(position, 1.0);
    }else{
		vertex.v_StaticLight = vec4(0.0,0.0,0.0,0.0);
		vertex.v_PassThrough = false;
		gl_Position = vec4(0,0,0,0);
    }

	vertex.v_ColorOverlay = vec4(1,1,1,1);
	if((int(tnts[gl_InstanceID].secondsUntilExplode*4.35)&1)==0){
		vertex.v_ColorOverlay = vec4(1,1,1,0.247);
	}
}

#type geometry
#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vData{
	vec2 v_TexCoord;
	vec4 v_StaticLight;
	vec4 v_ColorOverlay;
	bool v_PassThrough;
} vertices[];

out fData{
	vec2 v_TexCoord;
	vec4 v_StaticLight;
	vec4 v_ColorOverlay;
} frag;


void main() {
	bool passThrough = vertices[0].v_PassThrough;
	if (passThrough) {
		for (int i = 0; i < gl_in.length(); i++) {
		    gl_Position = gl_in[i].gl_Position;
		    frag.v_TexCoord = vertices[i].v_TexCoord;
		    frag.v_StaticLight = vertices[i].v_StaticLight;
		    frag.v_ColorOverlay = vertices[i].v_ColorOverlay;
		    EmitVertex();
		}
	}
}

#type fragment
#version 430 core

out vec4 color;
in fData{
	vec2 v_TexCoord;
	vec4 v_StaticLight;
	vec4 v_ColorOverlay;
} frag;

uniform sampler2D u_Texture;

void main()
{
    if(frag.v_StaticLight.a==0){
        discard;
    }
	color = texture(u_Texture,frag.v_TexCoord)*frag.v_StaticLight;
	color.rgb = mix(frag.v_ColorOverlay.rgb,color.rgb,frag.v_ColorOverlay.a);
}
