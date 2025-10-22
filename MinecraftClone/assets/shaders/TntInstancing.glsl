#type vertex
#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
	
out vec2 TexCoords;
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
	
void main()
{

    const int size = 100;
    int xOffset = gl_InstanceID%size;
    int yOffset = (gl_InstanceID/size)%size;
    int zOffset = gl_InstanceID/(size*size);
    TexCoords = aTexCoords;
    vec3 position = aPos+vec3(xOffset,yOffset,zOffset);
    gl_Position = u_ViewProjection*u_Transform*vec4(position, 1.0);
}


#type fragment
#version 430 core
out vec4 FragColor;
	
in vec2 TexCoords;
	
uniform sampler2D tex;
	
void main()
{             
    vec3 texCol = texture(tex, TexCoords).rgb;      
    FragColor = vec4(texCol, 1.0);
}
