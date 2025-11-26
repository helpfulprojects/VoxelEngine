#type compute
#version 430 core
#includeGlobalSource

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buffer0
{
    Chunk chunksData[];
};

layout(std430, binding = 6) buffer buffer6
{
    TntEntity tnts[];
};
uniform float u_DeltaTime;

void main()
{
    uint index = gl_GlobalInvocationID.x;
    if (index >= TNT_COUNT) {
        return;
    }
	tnts[index].position = vec3(0);
	tnts[index].velocity = vec3(0);
	tnts[index].visible = false;
	tnts[index].justBlewUp = false;
}
