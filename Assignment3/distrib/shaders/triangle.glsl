
#ifdef VertexShader

uniform mat4 worldToClip;

// Vertex data
layout(location=0) in vec4 pos;
layout(location=1) in vec4 normal;

// Outputs to fragment shader
smooth out vec3 n;

void main()
{
	n = normal.xyz;
	gl_Position = worldToClip * vec4(pos.xyz, 1.0);
}

#endif

#ifdef FragmentShader

// Inputs from vertex shader
smooth in vec3 n;
out vec4 col;

void main()
{
	// EXTRA: fill in your triangle shading implementation (below is just placeholder)
	col = vec4(n, 1.0);
}

#endif
