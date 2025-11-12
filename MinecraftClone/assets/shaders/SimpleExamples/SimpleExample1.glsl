#type vertex
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = u_ViewProjection*u_Transform*vec4(aPos.x, aPos.y, aPos.z, 1.0);
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
