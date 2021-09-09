#define _CRT_SECURE_NO_WARNINGS

#include "Shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace FW;
using namespace std;

void ReadFile(string filename, string &source)
{
	ifstream in(filename);
	if (in.good())
		source = string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	else
	{
		std::cout << "Could not open file " << filename << "!\n";
		__debugbreak();
	}
}

FW::Shader::Shader(const std::string & name, const bool compute, GLContext *ctx)
{
	std::cout << "Compiling shader " << name << "...";

	if (compute)
	{
		// Read shader source to string
		string source;
		ReadFile("shaders/" + name + ".glsl", source);

		// Add preprocess definitions to make line numbers match text editor and allow multiple shaders per file (though not necessary with compute shaders)
		source = (string)"#version 430\n" + (string)"#define ComputeShader\n" + (string)"#line 1\n" + source;

		// Create program from string
		program = new GLContext::Program(source.c_str());
	}
	else
	{
		// Read shader source to string
		string source;
		ReadFile("shaders/" + name + ".glsl", source);

		// Add preprocess definitions to make line numbers match text editor and allow multiple shaders per file
		string vertSource = (string)"#version 430\n" + (string)"#define VertexShader\n" + (string)"#line 1\n" + source;
		string fragSource = (string)"#version 430\n" + (string)"#define FragmentShader\n" + (string)"#line 1\n" + source;
		
		// Create program from strings
		program = new GLContext::Program(vertSource.c_str(), fragSource.c_str());
	}

	ctx->setProgram(name.c_str(), program);
	handle = program->getHandle();
	initialized = true;

	std::cout << " Done!\n";
}

FW::Shader::~Shader()
{
}
