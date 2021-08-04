
#ifdef VertexShader

uniform mat4 worldToClip;

// Vertex data
layout(location = 0) in vec4 pos;
layout(location = 1) in vec4 color;

// Outputs to fragment shader
flat out vec4 fragCol;

void main()
{
	fragCol = color;
	gl_Position = worldToClip * vec4(pos.xyz, 1.0);
}

#endif

#ifdef FragmentShader

// Inputs from vertex shader
flat in vec4 fragCol;
out vec4 col;

void main()
{
	// EXTRA: fill in your triangle shading implementation (below is just placeholder)
	col = fragCol;
}

#endif
