#type vertex
layout (location = 0) in vec3 aPos;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_ViewProjection*u_Transform*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}


#type fragment
#version 430 core
out vec4 FragColor;

uniform sampler2D tex;
void main()
{
    FragColor = vec4(1.0,1.0,1.0, 1.0);
} 
