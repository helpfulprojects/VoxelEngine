#type vertex
#version 430 core
#includeGlobalSource

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};

out vData{
	vec2 v_TexCoord;
	vec4 v_StaticLight;
	vec4 v_ColorOverlay;
	bool v_PassThrough;
} vertex;
#line 0
void main()
{
	const int index = gl_VertexID;
	bool shouldDraw = tnts[gl_VertexID].visible;
	vertex.v_PassThrough = shouldDraw;
	vertex.v_ColorOverlay = vec4(1,1,1,1);
	if(shouldDraw){
		vec3 position = tnts[gl_VertexID].position;
		gl_Position = vec4(position, 1.0);
		if((int(tnts[gl_VertexID].secondsUntilExplode*4.35)&1)==0){
			vertex.v_ColorOverlay = vec4(1,1,1,0.247);
		}
	}else{
		gl_Position = vec4(0,0,0,0);
	}
}

#type geometry
#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 14) out;

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
vec4 EAST_WEST_LIGHT = vec4(0.8,0.8,0.8,1.0);
vec4 SOUTH_NORTH_LIGHT = vec4(0.7,0.7,0.7,1.0);
vec4 TOP_BOTTOM_LIGHT = vec4(1.0,1.0,1.0,1.0);
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
#line 0
void main() {
	bool passThrough = vertices[0].v_PassThrough;
	if (passThrough) {
		vec4 center = gl_in[0].gl_Position;

		vec4 p1 = u_ViewProjection * u_Transform * (center + vec4(1,1,1,0));
		vec4 p2 = u_ViewProjection * u_Transform * (center + vec4(0,1,1,0));
		vec4 p3 = u_ViewProjection * u_Transform * (center + vec4(1,0,1,0));
		vec4 p4 = u_ViewProjection * u_Transform * (center + vec4(0,0,1,0));

		vec4 p5 = u_ViewProjection * u_Transform * (center + vec4(1,1,0,0));
		vec4 p6 = u_ViewProjection * u_Transform * (center + vec4(0,1,0,0));
		vec4 p7 = u_ViewProjection * u_Transform * (center + vec4(1,0,0,0));
		vec4 p8 = u_ViewProjection * u_Transform * (center + vec4(0,0,0,0));

		gl_Position = p1;
		frag.v_TexCoord = vec2(0.625,0.375);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p2;
		frag.v_TexCoord = vec2(0.50,0.375);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p3;
		frag.v_TexCoord = vec2(0.625,0.25);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p4;
		frag.v_TexCoord = vec2(0.50,0.25);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p8;
		frag.v_TexCoord = vec2(0.50,0.125);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p2;
		frag.v_TexCoord = vec2(0.375,0.25);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p6;
		frag.v_TexCoord = vec2(0.375,0.125);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p1;
		frag.v_TexCoord = vec2(0.25,0.25);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   


		gl_Position = p5;
		frag.v_TexCoord = vec2(0.25,0.125);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p3;
		frag.v_TexCoord = vec2(0.125,0.25);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p7;
		frag.v_TexCoord = vec2(0.125,0.125);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p8;
		frag.v_TexCoord = vec2(0.0,0.125);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   

		gl_Position = p5;
		frag.v_TexCoord = vec2(0.125,0.0);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		gl_Position = p6;
		frag.v_TexCoord = vec2(0.0,0.0);
		frag.v_ColorOverlay = vertices[0].v_ColorOverlay;
		EmitVertex();   
		EndPrimitive();
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

layout (binding = 1) uniform sampler2D u_Texture;

void main()
{
	color = texture(u_Texture,frag.v_TexCoord);
	color.rgb = mix(frag.v_ColorOverlay.rgb,color.rgb,frag.v_ColorOverlay.a);
}
