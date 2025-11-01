#type compute

layout (local_size_x = 1, local_size_y = 2, local_size_z = 1) in;

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
	uint index = gl_WorkGroupID.x*2+gl_LocalInvocationIndex;

	if(tnts[index].visible){
		vec3 chunkPosition = floor(tnts[index].position/16);
		vec3 actualChunkPosition = chunkPosition*16;
		ivec3 localPos = ivec3(tnts[index].position-chunkPosition*16);
		int chunkIndex = int(chunkPosition.x+chunkPosition.y*WORLD_WIDTH+chunkPosition.z*WORLD_WIDTH*WORLD_HEIGHT);
		if(chunkPosition.y>=WORLD_HEIGHT || chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]==0){
			tnts[index].velocity.y = tnts[index].velocity.y+GRAVITY*u_DeltaTime;
			tnts[index].position = tnts[index].position+tnts[index].velocity*u_DeltaTime;
			if(tnts[index].position.y<0){
				tnts[index].visible = false;
			}
		}else if(chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]!=0){
			tnts[index].visible = false;
			chunksData[chunkIndex].explosions[localPos.x][localPos.y][localPos.z] = TNT_EXPLOSION_STRENGTH;
			chunksData[chunkIndex].hasExplosion = true;
		}
	}
}
