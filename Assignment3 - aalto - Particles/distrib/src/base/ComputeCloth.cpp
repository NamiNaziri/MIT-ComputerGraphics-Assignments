#define _CRT_SECURE_NO_WARNINGS

#include "ComputeCloth.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace FW;
using namespace std;

#define RESET_X 16
#define RESET_Y 16

#define EVAL_X 16
#define EVAL_Y 1
#define EVAL_Z 12

FW::ComputeCloth::ComputeCloth(GLContext* ctx) : ctx(ctx)
{
	InitGL();
	Resize(60, 60);
}

void FW::ComputeCloth::InitGL()
{
	// Initialize graphics shaders
	triangleProg = Shader("triangle", false, ctx);
	lineProg = Shader("line", false, ctx);
	pointProg = Shader("point", false, ctx);

	// And compute shaders
	genRenderDataProg = Shader("genRenderData", true, ctx);

	resetProg = Shader("reset", true, ctx);
	evalFProg = Shader("evalF", true, ctx);
	BxcPlusDProg = Shader("BxcPlusD", true, ctx);

	// Generate storage buffers
	glGenBuffers(1, &state);
	glGenBuffers(1, &nextState);
	glGenBuffers(1, &tempStateA);
	glGenBuffers(1, &tempStateB);
	glGenBuffers(1, &brokenSprings);

	// Generate vertex buffers
	glGenBuffers(2, particleVBO);

	// Generate VAOs
	glGenVertexArrays(1, &particleVAO);

	// Generate uniform buffer
	glGenBuffers(1, &clothUBO);

	// Fill in VAO data
	glBindVertexArray(particleVAO);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[0]);
	glVertexAttribPointer(0, 4, GL_FLOAT, false, sizeof(Vec4f), 0); // Note that in openGL, an array of vec3s has a stride of four floats

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[1]);
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vec4f), 0);

}

void FW::ComputeCloth::UploadClothUBO()
{
	glBindBuffer(GL_UNIFORM_BUFFER, clothUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ClothParams), &params, GL_STATIC_DRAW);
}

void FW::ComputeCloth::Resize(int w, int h)
{
	params.w = w;
	params.h = h;

	int particleCount = w*h;

	// Note that state takes up two vec4's of data per particle due to alignment
	int stateSize = w*h * 8 * sizeof(float);

	params.springK = 30 * w * h;
	params.scale = 2.0f / h;

	//Resize state buffers
	glBindBuffer(GL_ARRAY_BUFFER, state);
	glBufferData(GL_ARRAY_BUFFER, stateSize, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, nextState);
	glBufferData(GL_ARRAY_BUFFER, stateSize, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tempStateA);
	glBufferData(GL_ARRAY_BUFFER, stateSize, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tempStateB);
	glBufferData(GL_ARRAY_BUFFER, stateSize, 0, GL_STATIC_DRAW);

	// Resize render data
	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(Vec4f), 0, GL_STATIC_DRAW); // Note that in openGL, an array of vec3s has a stride of four floats

	glBindBuffer(GL_ARRAY_BUFFER, particleVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(Vec4f), 0, GL_STATIC_DRAW); // Same as above
	Reset();
}

void FW::ComputeCloth::Reset()
{

	vector<int> brokenData(params.w*params.h * 12, 0);
	glBindBuffer(GL_ARRAY_BUFFER, brokenSprings);
	glBufferData(GL_ARRAY_BUFFER, brokenData.size() * sizeof(int), brokenData.data(), GL_STATIC_DRAW);

	resetProg.program->use();

	UploadClothUBO();

	int x = (params.w + RESET_X - 1) / RESET_X;
	int y = (params.h + RESET_Y - 1) / RESET_Y;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, clothUBO);
	glDispatchCompute(x, y, 1);
}

void FW::ComputeCloth::FireBullet(Vec4f origin, Vec4f dir)
{
	// YOUR CODE HERE (bullet extra):
	// Initialize bullet position and velocity here in case you want to do the bullet firing extra.

}

// Evaluate derivative of state B: A = dB/dt;
void FW::ComputeCloth::EvalF(float dt, GLuint A, GLuint B)
{
	evalFProg.program->use();

	int x = (params.w + EVAL_X - 1) / EVAL_X;
	int y = (params.h + EVAL_Y - 1) / EVAL_Y;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, A);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, B);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, brokenSprings);

	params.dt = dt;
	UploadClothUBO();

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, clothUBO);
	glDispatchCompute(x, y, 1);
}

// For states A, B, D, and float c, compute: A = B * dt + C (used for Euler step with flexibility of destination state)
void FW::ComputeCloth::BxcPlusD(GLuint A, GLuint B, float dt, GLuint C)
{
	BxcPlusDProg.program->use();

	int x = (params.w + RESET_X - 1) / RESET_X;
	int y = (params.h + RESET_Y - 1) / RESET_Y;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, A);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, B);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, C);

	params.dt = dt;
	UploadClothUBO();

	glBindBufferBase(GL_UNIFORM_BUFFER, 3, clothUBO);
	glDispatchCompute(x, y, 1);
}

// Copy size bytes from read to write
void CopyBuffer(GLuint read, GLuint write, int size)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, write);
	glBindBuffer(GL_ARRAY_BUFFER, read);
	glCopyBufferSubData(GL_ARRAY_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, size);
}

void FW::ComputeCloth::Advance(float dt)
{
	// Size of state object in bytes (note that due to alignment, it takes up two vec4's worth of space even though it contains only two vec3's)
	int bytes = 8 * params.w * params.h * sizeof(float);

	// Your code here (compute extra):
	// Implement an integrator of your choice. RK4 or some other higher-order method is recommended, otherwise you'll likely run into stability issues
	// Use evalF to evaluate the derivative of a state, and BxcPlusD to take Euler steps with evaluated derivatives. You can think of the GLuints as
	// opaque pointers to data arrays, and treat this like any other CPU-side algorithm. Below is an Euler integrator to give you an idea of how the
	// methods work.

	//EvalF(dt, tempStateA, state);							// A = evalF(state)
	//BxcPlusD(state, tempStateA, dt, state);				// state = A * dt + state

}

void FW::ComputeCloth::Render(bool shading_toggle, const Mat4f& worldToClip)
{
	// First, run compute shader that generates render data
	genRenderDataProg.program->use();

	int x = (params.w + RESET_X - 1) / RESET_X;
	int y = (params.h + RESET_Y - 1) / RESET_Y;

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, state);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleVBO[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, particleVBO[1]);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, clothUBO);

	glDispatchCompute(x, y, 1);

	// Set triangle draw program as active and set its uniforms
	pointProg.program->use();
	glUniformMatrix4fv(triangleProg.program->getUniformLoc("worldToClip"), 1, false, worldToClip.getPtr());

	// Bind VBO and VAO
	glBindVertexArray(particleVAO);

	// Draw outputted data as points. You'll have to change this if you want to render the cloth as triangles
	glPointSize(2.0f);
	glDrawArrays(GL_POINTS, 0, params.w * params.h);

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glVertex3f(params.bulletPos.x, params.bulletPos.y, params.bulletPos.z);
	glEnd();
}

