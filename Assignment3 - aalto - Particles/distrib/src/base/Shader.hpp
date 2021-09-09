#pragma once

#include "base/Math.hpp"
#include "gpu/GLContext.hpp"
#include <string>

#define COMPUTE_CLOTH_MODULE

namespace FW {

	// Simple helper to manage multiple shaders more easily
	struct Shader
	{
		Shader() {};
		Shader(const std::string& name, const bool compute, GLContext *ctx);
		~Shader();

		GLuint handle;
		GLContext::Program *program;
		bool initialized = false;
	};

} // namespace FW
