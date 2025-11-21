#type vertex
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in float aOffset;

out vec2 texCoords;
out float offsets;
uniform mat4 u_ViewProjection;
uniform mat4 u_View;
uniform mat4 u_Projection;


void main()
{
    texCoords = aTexCoords;
    offsets = aOffset;
    gl_Position = u_ViewProjection*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#type geometry
#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 texCoords[];
in float offsets[];

out vec2 TexCoords; 

uniform float time;
vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

vec4 explode(vec4 position, vec3 normal, float id)
{
//    float magnitude = 2.0;
//    float speed = 1.0;   
//
//    vec3 startOffset = normal * magnitude*(id*50+10);
//
//	float tExp = exp(-time * speed);
//	float tLinear = 1.0 - time * 0.05;  // fallback linear motion
//	float t = max(tExp, tLinear);
//	t = clamp(t, 0.0, 1.0);
//
//    vec3 direction = mix(vec3(0.0), startOffset, t);
//
//    return position + vec4(direction, 0.0);

    float magnitude = 10.0;
    float duration = 3;
	float t = clamp(time-(id/11.0)*5.00,0, duration);
    //vec3 change =mix(normal*magnitude,vec3(0.0), t/4.0);
    float alpha = t/duration;
    vec3 dir = normal*magnitude;
    vec3 change = dir - dir*(alpha*alpha*alpha);
    return position+vec4(change,0.5);
}

uniform mat4 u_View;
uniform mat4 u_Transform;
uniform mat4 u_Projection;
void main() {    
    vec3 normal = GetNormal();
    float offset = offsets[0];

    //gl_Position = u_Transform*explode(gl_in[0].gl_Position, normal,offset);
    gl_Position = u_Transform*explode(gl_in[0].gl_Position, normal,offset);
    TexCoords = texCoords[0];
    EmitVertex();
    gl_Position = u_Transform*explode(gl_in[1].gl_Position, normal,offset);
    TexCoords = texCoords[1];
    EmitVertex();
    gl_Position = u_Transform*explode(gl_in[2].gl_Position, normal,offset);
    TexCoords = texCoords[2];
    EmitVertex();
    EndPrimitive();
}  


#type fragment
#version 430 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D tex;
void main()
{
    vec3 texCol = texture(tex, TexCoords).rgb;      
    FragColor = vec4(texCol, 1.0);
} 
