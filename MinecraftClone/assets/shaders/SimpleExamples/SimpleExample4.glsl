#type vertex
#version 430 core

layout(std430, binding = 0) readonly buffer buffer0
{
	vec2 quadPositions[]; 
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
uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

int indices[4] = {0, 1, 3, 2};

void main()
{
	const int index = gl_VertexID/4;
	const int currVertexID = gl_VertexID % 4;
    vec3 position = vec3(quadPositions[index],0);
	position += facePositions[4][indices[currVertexID]];
    gl_Position = u_ViewProjection*u_Transform*vec4(position, 1.0);
}


#type fragment
#version 430 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.2f, 1.0f, 0.5f, 1.0f);
} 
