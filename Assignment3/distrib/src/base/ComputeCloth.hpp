#pragma once

#include "base/Math.hpp"
#include "gpu/GLContext.hpp"
#include "Shader.hpp"
#include <string>

#define COMPUTE_CLOTH_MODULE

namespace FW {

	struct ClothParams
	{
		float springK = 30;
		float springDamp = 1.48f;
		float dragK = 1.7f;
		float windStrength = .2f;
		float springBreakThreshold = 1.27f;
		float scale = 1.0f;
		float dt = .1f;

		float bulletR = .2f;
		Vec4f bulletPos = 99;
		Vec4f bulletVel = 0;

		int w = 60, h = 60;
	};

	class ComputeCloth
	{
		
	public:
		// Public methods

		// Class constructors
		ComputeCloth() {};
		ComputeCloth(GLContext* ctx);
		
		// Initializes GL buffers and system state
		void InitGL();

		// Uploads cloth UBO in case parameters have changed
		void UploadClothUBO();

		// Resize cloth system to new dimensions
		void Resize(int w, int h);

		// Reset cloth state
		void Reset();

		// Take one time step
		void Advance(float dt);

		// Render cloth
		void Render(bool shading_toggle, const FW::Mat4f& worldToClip);

		void FireBullet(Vec4f origin, Vec4f dir);

	private:
		// Private methods

		// Evaluate derivative of state B: A = dB/dt;
		void EvalF(float dt, GLuint A, GLuint B);

		// For states A, B, D, and float c, compute: A = B * dt + C (used for Euler step with flexibility of destination state)
		void BxcPlusD(GLuint A, GLuint B, float dt, GLuint D);

	public:
		// Public members

		// Cloth simulation parameters
		ClothParams params;

		// Storage buffers
		GLuint state; // Stores current state of cloth system
		GLuint nextState, tempStateA, tempStateB; // Temporary states used for RK4 in example solution
		GLuint brokenSprings; // Stores brokenness status for each spring

		// Vertex buffers used in rendering
		GLuint particleVBO[2];

		// Vertex array objects used in rendering
		GLuint particleVAO;

		// Render program handles
		Shader triangleProg, lineProg, pointProg;

		// Compute program handles
		Shader genRenderDataProg, resetProg, evalFProg, BxcPlusDProg;

		// Uniform buffer containing cloth parameters etc.
		GLuint clothUBO;
		

	private:
		// Private members

		GLContext* ctx;
	};

} // namespace FW
