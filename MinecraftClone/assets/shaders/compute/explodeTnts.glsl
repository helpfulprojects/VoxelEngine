#type compute
#version 430 core
#includeGlobalSrouce

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

void main() {
	uint index = gl_GlobalInvocationID.x;
	if(index>=TNT_COUNT){
		return;
	}

	if(tnts[index].visible){
		tnts[index].secondsUntilExplode -= u_DeltaTime;
		if(tnts[index].secondsUntilExplode<=0.0){
			vec3 chunkPosition = floor(tnts[index].position/16);
			ivec3 localPos = ivec3(tnts[index].position-chunkPosition*16);
			int chunkIndex = int(chunkPosition.x+chunkPosition.y*WORLD_WIDTH+chunkPosition.z*WORLD_WIDTH*WORLD_HEIGHT);
			tnts[index].visible = false;
			chunksData[chunkIndex].explosions[localPos.x][localPos.y][localPos.z] = index<<3 | TNT_EXPLOSION_STRENGTH;
			chunksData[chunkIndex].hasExplosion = true;
		}
	}
}
