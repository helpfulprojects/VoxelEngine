#type compute

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};
vec3 hash33(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return fract((p.xxy + p.yzz) * p.zyx);
}
#line 0
void main() {
	uint index = gl_WorkGroupID.x + gl_LocalInvocationIndex;
    const int size = 216;

    uint xOffset = index % size;
    uint yOffset = (index / size) % size;
    uint zOffset = index / (size * size);
    tnts[index].position = DEFAULT_SPAWN + vec3(xOffset, yOffset, zOffset);

    float seed = float(index);

	vec3 rand = hash33(vec3(index, index + 1.0, index + 2.0));
	vec3 randomDir = normalize(rand * 2.0 - 1.0);
	tnts[index].secondsUntilExplode = 5.0+rand.x*5;

    float speed = 35; 
    tnts[index].velocity = randomDir * speed;
//    if(index>(1000000-2)){ 
//		tnts[index].visible = true;
//    }else{
//		tnts[index].visible = false;
//    }
//
	tnts[index].visible = true;
}
