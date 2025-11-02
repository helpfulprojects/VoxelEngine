#type vertex

layout (location=0) in vec4 position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

uniform mat4 u_View;
uniform mat4 u_Projection;

//const float VIEW_SHRINK = 1.0 - (1.0 / 256.0);
const float VIEW_SHRINK = 1.0 - (1.0 / (256.0 * 16.0));
const mat4 VIEW_SCALE = mat4(
    VIEW_SHRINK, 0.0, 0.0, 0.0,
    0.0, VIEW_SHRINK, 0.0, 0.0,
    0.0, 0.0, VIEW_SHRINK, 0.0,
    0.0, 0.0, 0.0, 1.0
);
void main()
{
    vec4 positionRelativeToCam = u_View*u_Transform*position;
    gl_Position = u_Projection * VIEW_SCALE * positionRelativeToCam;
}












#type fragment
#version 330 core

out vec4 fragColor;

void main()
{
    fragColor = vec4(0.0,0.0,0.0,1.0);
}
