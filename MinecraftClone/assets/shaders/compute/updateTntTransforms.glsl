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
		vec3 chunkPosition = floor(tnts[index].position/16);
		ivec3 localPos = ivec3(tnts[index].position-chunkPosition*16);
		int chunkIndex = int(chunkPosition.x+chunkPosition.y*WORLD_WIDTH+chunkPosition.z*WORLD_WIDTH*WORLD_HEIGHT);
		uint explosionValue = bitfieldExtract(chunksData[chunkIndex].explosions[localPos.x][localPos.y][localPos.z],0,3);
		if(explosionValue!=0 && !tnts[index].justBlewUp){
			uint otherTntId = bitfieldExtract(chunksData[chunkIndex].explosions[localPos.x][localPos.y][localPos.z],3,29);
			vec3 otherTntPosition = tnts[otherTntId].position;
			vec3 diff = tnts[index].position - otherTntPosition;
			vec3 diffDirection = normalize(diff);
			float diffLength = length(diff);
			float strength = 50.0 / (diffLength * diffLength + 1.0); 
			tnts[index].velocity += diffDirection * strength;
		}
		if(chunkPosition.y>=WORLD_HEIGHT || chunksData[chunkIndex].blockTypes[localPos.x][localPos.y][localPos.z]==0){
			tnts[index].velocity.y = tnts[index].velocity.y+GRAVITY*u_DeltaTime;
			tnts[index].position = tnts[index].position+tnts[index].velocity*u_DeltaTime;
			if(tnts[index].position.y<0){
				tnts[index].visible = false;
			}
		}
		if(tnts[index].justBlewUp){
			tnts[index].justBlewUp = false;
		}
	}
}
