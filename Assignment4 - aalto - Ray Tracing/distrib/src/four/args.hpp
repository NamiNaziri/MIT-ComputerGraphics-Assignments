#pragma once

#include <string>
#include <vector>

struct Args
{
	Args(const std::vector<std::string>& args);
	Args() {}
	void parse(const std::vector<std::string>& args);

	// Rendering output

	std::string input_file;
	std::string output_file;
	std::string depth_file;
	std::string normals_file;
	int		width;
	int		height;
	bool	stats;

	// Rendering options

	float	depth_min;
	float	depth_max;
	int		bounces;
	float	weight; 
	bool	transparent_shadows;
	bool	shadows;
	bool	shade_back;
	bool	display_uv;

	// Supersampling

	int	num_samples;
	int	sample_zoom;

	enum SamplePatternType {
		Pattern_Regular = 1,	// regular grid within the pixel
		Pattern_Uniform = 0,	// uniformly distributed random
		Pattern_Jittered = 2	// jittered within subpixels
	};
	SamplePatternType sampling_pattern;

	enum ReconstructionFilterType {
		Filter_Box		= 0,
		Filter_Tent		= 1,
		Filter_Gaussian	= 2
	};
	ReconstructionFilterType reconstruction_filter;

	float filter_radius;
	std::string samples_file;
	int	filter_zoom;
	std::string filter_file;

	// GUI options

	bool	gui;
	int		sphere_vert;
	int		sphere_horiz;
	bool	gouraud_shading;
	bool	specular_fix;
	bool	show_progress;
};

class RayTracer; class SceneParser;
typedef unsigned GLuint;
GLuint render(RayTracer& rt, SceneParser& scene, const Args& args);
