#ifdef ComputeShader

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

struct clothState
{
	vec3 pos, vel;
};

layout (std430, binding=0) buffer cloth_state
{ 
	clothState state[];
};

layout (std430, binding=1) buffer vbo1
{ 
	vec4 pos[];
};

layout (std430, binding=2) buffer vbo2
{ 
	vec4 col[];
};

layout(std140, binding=3) uniform params
{
	float springK;
	float springDamp;
	float dragK;
	float windStrength;
	float springBreakThreshold;
	float scale;
	float dt;

	// Bullet projectile data
	float bulletR;
	vec4 bulletPos;
	vec4 bulletVel;
	
	int w, h;
};

void main() 
{
	int idx = int(gl_GlobalInvocationID.y * w + gl_GlobalInvocationID.x);

	if(gl_GlobalInvocationID.y < h && gl_GlobalInvocationID.x < w)
	{
		// Currently only point data is generated. You can adapt this shader to generate triangle data and normals too, but that will require some CPU-side modification too.
		pos[idx] = vec4(state[idx].pos, 1);
		col[idx] = vec4(1);
	}
}

#endif
