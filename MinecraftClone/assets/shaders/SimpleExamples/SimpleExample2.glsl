#type vertex
#version 430 core
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

void main()
{
    FragColor = vec4(0.5f, 0.2f, 1.0f, 1.0f);
} 
