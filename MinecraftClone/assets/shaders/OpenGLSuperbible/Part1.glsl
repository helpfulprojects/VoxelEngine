#type vertex
#version 450 core

void main()
{
    gl_Position = vec4(0.0,0.0,0.5,1.0);
}


#type fragment
#version 450 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0,0.8,1.0,1.0);
} 
