#ifdef ComputeShader

#define SIZE_X 16
#define SIZE_Y 1
#define SIZE_Z 12

layout (local_size_x = 16, local_size_y = 1, local_size_z = 12) in;

struct clothState
{
	vec3 pos, vel;
};

// Write results into this state
layout (std430, binding=0) buffer state_a
{ 
	clothState result[];
};

// Evaluate derivative of this state
layout (std430, binding=1) buffer state_b
{ 
	clothState state[];
};

// Contains brokennes flag for each spring
layout(std430, binding = 2) buffer brokenSSBO
{
	int broken[];
};

// Uniform block containing cloth system parameters
layout(std140, binding=3) uniform params
{
	float springK;
	float springDamp;
	float dragK;
	float windStrength; // Wind force multiplier
	float springBreakThreshold; // How far from rest length spring can extend without breaking
	float scale; // Scale of cloth springs
	float dt; // Current timestep

	// Bullet projectile data
	float bulletR;
	vec4 bulletPos;
	vec4 bulletVel;
	
	// Cloth dimensions
	int w, h;
};

const float mass = 1.0f;

// Table containing index offsets for each spring connected to a particle
const int springs[] = {
	1, 0,
	-1, 0,
	0, 1,
	0, -1,
	1, 1,
	-1, -1,
	1, -1,
	-1, 1,
	2, 0,
	-2, 0,
	0, 2,
	0, -2
};


// spring lenghts before taking scale into account. 
float springLengths[] = { 1.0f, sqrt(2.0f), 2.0f };

// Using a shared memory block allows us to compute spring forces in parallel for all 12 springs connected to a single particle
shared vec3 forces[SIZE_X * SIZE_Z];

// The given local block sizes mean that we will have blocks of 16x1x12 threads. Each block handles 16 particles, and 
// each particle has 12 threads assiciated with it, one thread for each spring. This shader should evaluate each spring
// force separately and write the result into shared memory, at which point the first thread of each particle computes
// the resulting acceleration and writes it to global memory. This may not be the fastest way to do it, and you should feel
// free to experiment with other threading schemes.
void main() 
{
	// Shorthands for easier use
	int x = int(gl_GlobalInvocationID.x);
	int y = int(gl_GlobalInvocationID.y);
	int z = int(gl_LocalInvocationID.z);

	// Index of current particle in state table
	int i = y * w + x;

	if (x < w && y < h) // Make sure we're inside bounds
	{
		// Initialize force of current thread to zero
		forces[SIZE_Z * gl_LocalInvocationID.x + z] = vec3(0);

		// Read position and velocity beforehand for easy access
		vec3 pos = state[i].pos;
		vec3 vel = state[i].vel;

		// spring end point coordinates
		int end_x = x + springs[z * 2 + 0];
		int end_y = y + springs[z * 2 + 1];

		// spring end point particle index in state table
		int end_i = end_y * w + end_x;

		if (end_x >= 0 && end_x < w && end_y >= 0 && end_y < h)
		{

			// YOUR CODE HERE (compute cloth):
			// Evaluate spring force for each spring in the cloth and write it to shared memory. 

			// forces[SIZE_Z * gl_LocalInvocationID.x + z] = ...

		}

		// Sync our thread block. All threads in the current block will wait here until every thread has reached this point
		// After this sync we know that all spring forces have been computed and we can proceed to write results
		barrier();

		// Only first thread for each particle in z-direction should write to global memory
		if (z == 0)
		{
			vec3 acceleration = vec3(0, -9.81, 0) * mass; // G = g*m

			// YOUR CODE HERE (compute cloth):
			// Sum together the forces computed earlier to get acceleration for current particle, then write derivatives of position and velocity
			// to the results buffer.

		}
	}
}

#endif
