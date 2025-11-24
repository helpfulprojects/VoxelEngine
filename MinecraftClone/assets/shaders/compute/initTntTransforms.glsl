#type compute
#version 430 core
#includeGlobalSrouce

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};
#line 0
void main() {
	uint index = gl_WorkGroupID.x + gl_LocalInvocationIndex;
    const int size = 216;

    uint xOffset = index % size;
    uint yOffset = (index / size) % size;
    uint zOffset = index / (size * size);
    tnts[index].position = DEFAULT_SPAWN + vec3(xOffset, yOffset, zOffset);
    tnts[index].position.y += 20;
    int offset = 100;
    tnts[index].position.x -= offset;
    tnts[index].position.z -= offset;

    float seed = float(index);

	vec3 rand = hash33(vec3(index, index + 1.0, index + 2.0));
	vec3 randomDir = normalize(rand * 2.0 - 1.0);

	vec3 rand2 = hash33(tnts[index].position);
	tnts[index].secondsUntilExplode = 4.0+fract(rand2.x+rand2.y+rand2.z);

    float speed = 45; 
    tnts[index].velocity = randomDir * speed;
    tnts[index].visible = true;
}
