#type compute

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};

layout(std430, binding = 6) buffer buffer6
{
	TntEntity tnts[];
};
uniform float u_DeltaTime;
uniform float u_LinePosition;

void main() {
	//uint index = gl_WorkGroupID.x*2+gl_LocalInvocationIndex;
	uint index = gl_GlobalInvocationID.x;
	if(index>=TNT_COUNT){
		return;
	}
	tnts[index].visible = tnts[index].position.y>=u_LinePosition;
}
