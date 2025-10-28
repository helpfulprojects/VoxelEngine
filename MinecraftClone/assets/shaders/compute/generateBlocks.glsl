#type compute

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buffer0 
{
	Chunk chunksData[]; 
};

layout(std430, binding = 1) buffer buffer1 
{
	ChunkQuads chunksQuads[]; 
};
layout(std430, binding = 4) buffer buffer4
{
	uint debugBuffer[];
};

ivec3 offsets[6] = ivec3[6](
    ivec3(0, 1, 0),
    ivec3(0, -1, 0),
    ivec3(1, 0, 0),
    ivec3(-1, 0, 0),
    ivec3(0, 0, 1),
    ivec3(0, 0, -1)
);
//
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@gmail.com)
// Version: 2024-11-07
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/stegu/webgl-noise
//

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+10.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);

  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));

  float n00 = norm.x * dot(g00, vec2(fx.x, fy.x));
  float n01 = norm.y * dot(g01, vec2(fx.z, fy.z));
  float n10 = norm.z * dot(g10, vec2(fx.y, fy.y));
  float n11 = norm.w * dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}



void main() {
	uint chunkIndex = gl_WorkGroupID.x+gl_WorkGroupID.y*WORLD_WIDTH+gl_WorkGroupID.z*WORLD_WIDTH*WORLD_HEIGHT;
	chunksData[chunkIndex].shouldRedraw = true;
	int index = 0;

	float freq = 0.005;     
	float amplitude = 20.0;     
	float baseHeight = 150.0;

	for(int x=0;x<CHUNK_WIDTH;x++){
		for(int z=0;z<CHUNK_WIDTH;z++){
			uint chunkX = gl_WorkGroupID.x * CHUNK_WIDTH;
			uint chunkZ = gl_WorkGroupID.z * CHUNK_WIDTH;
			chunksData[chunkIndex].x = chunkX;
			chunksData[chunkIndex].z = chunkZ;
			uint blockX = x+chunkX;
			uint blockZ = z+chunkZ;
			//NOISE
			float noise = cnoise(vec2(blockX, blockZ) * freq);
			int surfaceLevel = 14;

			for(int y=0;y<CHUNK_WIDTH;y++){
				uint chunkY = gl_WorkGroupID.y * CHUNK_WIDTH;
				uint blockY = y+chunkY;
				chunksData[chunkIndex].y = chunkY;
				if(blockY<surfaceLevel){
					chunksData[chunkIndex].blockTypes[x][y][z] = 1;
				}else if (blockY==surfaceLevel){
					chunksData[chunkIndex].blockTypes[x][y][z] = 2;
				} else{
					chunksData[chunkIndex].blockTypes[x][y][z] = 0;
				}

				chunksData[chunkIndex].explosions[x][y][z] = 0;
				if(gl_WorkGroupID.x == 1 && gl_WorkGroupID.z == 1 && x == 6 && z == 6 && y == 15){
					chunksData[chunkIndex].explosions[x][y][z] = TNT_EXPLOSION_STRENGTH;
				}
				chunksData[chunkIndex].hasExplosion = false;
			}
		}
	}
}

