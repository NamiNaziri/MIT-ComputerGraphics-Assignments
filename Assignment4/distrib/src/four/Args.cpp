#include "args.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

Args::Args(const vector<string>& args) :
	// rendering output
	input_file(""),
	output_file(""),
	depth_file(""),
	normals_file(""),
	width(100),
	height(100),
	stats(false),

	// rendering options
	depth_min(0),
	depth_max(1),
	bounces(0),
	weight(0.1),
	transparent_shadows(false),
	shadows(false),
	shade_back(false),

	// sampling
	num_samples(1),
	sample_zoom(10),
	sampling_pattern(Pattern_Regular),
	reconstruction_filter(Filter_Box),
	filter_radius(0.5),
	samples_file(""),
	filter_zoom(10),
	filter_file(""),

	// gui options
	gui(false),
	sphere_vert(5),
	sphere_horiz(10),
	gouraud_shading(false),
	specular_fix(false),
	show_progress(true),
	display_uv(false)
{
	parse(args);
}

void Args::parse(const vector<string>& args) {
	auto it = args.begin();
	while (it != end(args)) {
		// Rendering output
		if (*it == "-input") {
			input_file = *++it;
		} else if (*it == "-output") {
			output_file = *++it;
		} else if (*it == "-normals") {
			normals_file = *++it;
		} else if (*it == "-size") {
			width = stoi(*++it);
			height = stoi(*++it);
		} else if (*it == "-stats") {
			stats = true;
		}
		// Rendering options
		else if (*it == "-depth") {
			depth_min = stof(*++it);
			depth_max = stof(*++it);
			depth_file = *++it;
		} else if (*it == "-bounces") {
			bounces = stoi(*++it);
		} else if (*it == "-weight") {
			weight = stof(*++it);
		} else if (*it == "-transparent_shadows") {
			shadows = true;
			transparent_shadows = true;
		} else if (*it == "-shadows") {
			shadows = true;
		} else if (*it == "-shade_back") {
			shade_back = true;
		} else if (*it == "-uv") {
			display_uv = true;
		}
		// Supersampling
		else if (*it == "-uniform_samples") {
			sampling_pattern = Pattern_Uniform;
			num_samples = stoi(*++it);
		} else if (*it == "-regular_samples") {
			sampling_pattern = Pattern_Regular;
			num_samples = stoi(*++it);
		} else if (*it == "-jittered_samples") {
			sampling_pattern = Pattern_Jittered;
			num_samples = stoi(*++it);
		} else if (*it == "-box_filter") {
			reconstruction_filter = Filter_Box;
			filter_radius = stof(*++it);
		} else if (*it == "-tent_filter") {
			reconstruction_filter = Filter_Tent;
			filter_radius = stof(*++it);
		} else if (*it == "-gaussian_filter") {
			reconstruction_filter = Filter_Gaussian;
			filter_radius = stof(*++it);
		}
		// GUI options
		else if (*it == "-gui") {
			gui = true;
		} else if (*it == "-tessellation") {
			sphere_horiz = stoi(*++it);
			sphere_vert = stoi(*++it);
		} else if (*it == "-gouraud") {
			gouraud_shading = true;
		} else if (*it == "-specular_fix") {
			specular_fix = true;
		} else if (*it == "-show_progress") {
			show_progress = true;
		}
		else { assert(false && "Unknown argument!"); }
		++it;
	}
}
