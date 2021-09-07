
#include "Camera.h"
#include "ray.hpp"
#include "hit.hpp"
#include "SceneParser.h"
#include "args.hpp"
#include "lights.hpp"
#include "material.hpp"
#include "objects.hpp"
#include "raytracer.hpp"
#include "VecUtils.h"
#include "Film.h"
#include "Sampler.h"
#include "Filter.h"

#include "gui/Image.hpp"
#include "io/File.hpp"
#include "io/ImageLodePngIO.hpp"
#include "base/Main.hpp"

#include <atomic>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <numeric>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

using namespace std;
using namespace FW;

// The raytracer in this assignment is a command line application.
// While working on the assignment, if you want to run the raytracer from within Visual
// Studio, use the default argument string in main() to enter the arguments you want.
// For repeated tests, use test scripts like the ones we provide in /exe.
// These can be run simply by double-clicking them in Windows Explorer.
// Hint: if you need to see text output from the raytracer while running it under VS,
// one convenient way is to run it without debugging (Ctrl+F5). VS will then keep the
// console window open for you afterwards.
int main(int argcp, char *argvp[]) {
	if (argcp == 1) {
		// no command line arguments; launch interactive viewer
		fw_main(argcp, argvp);

		return 0;
	}

	auto arg = vector<string>(argvp + 1, argvp + argcp);
	// Parse the arguments
	auto args = Args(arg);
	// Parse the scene
	auto scene_parser = SceneParser(args.input_file.c_str());
	// Construct tracer
	auto ray_tracer = RayTracer(scene_parser, args);

	// If there is no scene, just display the UV coords
	if (!scene_parser.getGroup())
		args.display_uv = true;

	// Render; measure time
	auto start = chrono::steady_clock::now();
	render(ray_tracer, scene_parser, args);
	auto end = chrono::steady_clock::now();

	cout << "Rendered " << args.output_file << " in " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms." << endl;
	return 0;
}

GLuint render(RayTracer& ray_tracer, SceneParser& scene, const Args& args) {
	auto image_pixels = Vec2i(args.width, args.height);

	// Construct images
	unique_ptr<Image> image, depth_image, normals_image;
	if (!args.output_file.empty()) {
		image.reset(new Image(image_pixels, ImageFormat::RGBA_Vec4f));
		image->clear(Vec4f());
	}
	if (!args.depth_file.empty()) {
		depth_image.reset(new Image(image_pixels, ImageFormat::RGBA_Vec4f));
		depth_image->clear(Vec4f());
	}
	if (!args.normals_file.empty()) {
		normals_image.reset(new Image(image_pixels, ImageFormat::RGBA_Vec4f));
		normals_image->clear(Vec4f());
	}

	// EXTRA
	// The Filter and Film objects are not used by the starter code. They provide starting points
	// for implementing smarter supersampling, whereas the required type of less fancy supersampling
	// can be implemented by taking the average of the samples drawn from each pixel.
	// unique_ptr<Filter> filter(Filter::constructFilter(args.reconstruction_filter, args.filter_radius));
	// Film film(image.get(), filter.get()));

	// progress counter
	atomic<int> lines_done = 0;

	// Main render loop!
	// Loop through all the pixels in the image
	// Generate all the samples
	// Fire rays
	// Compute shading
	// Accumulate into image
	
	// Loop over scanlines.
	// #pragma omp parallel for // Uncomment this & enable OpenMP in project for parallel rendering (see handout)
	for (int j = 0; j < args.height; ++j) {
		// Print progress info
		if (args.show_progress) ::printf("%.2f%% \r", lines_done * 100.0f / image_pixels.y);

		// Construct sampler.
		auto sampler = unique_ptr<Sampler>(Sampler::constructSampler(args.sampling_pattern, args.num_samples));

		// Loop over pixels on a scanline
		for (int i = 0; i < args.width; ++i) {
			// Loop through all the samples for this pixel.
			for (int n = 0; n < args.num_samples; ++n) {
				// Get the offset of the sample inside the pixel. 
				// You need to fill in the implementation for this function when implementing supersampling.
				// The starter implementation only supports one sample per pixel through the pixel center.
				Vec2f offset = sampler->getSamplePosition(n);

				// Convert floating-point pixel coordinate to canonical view coordinates in [-1,1]^2
				// You need to fill in the implementation for Camera::normalizedImageCoordinateFromPixelCoordinate.
				Vec2f ray_xy = Camera::normalizedImageCoordinateFromPixelCoordinate(Vec2f(float(i), float(j)) + offset, image_pixels);

				// Generate the ray using the view coordinates
				// You need to fill in the implementation for this function.
				Ray r = scene.getCamera()->generateRay(ray_xy);

				// Trace the ray!
				Hit hit;
				float tmin = scene.getCamera()->getTMin();

				// You should fill in the gaps in the implementation of traceRay().
				// args.bounces gives the maximum number of reflections/refractions that should be traced.
				Vec3f sample_color = ray_tracer.traceRay(r, tmin, args.bounces, 1.0f, hit, Vec3f(1.0f));

				// YOUR CODE HERE (R9)
				// This starter code only supports one sample per pixel and consequently directly
				// puts the returned color to the image. You should extend this code to handle
				// multiple samples per pixel. Also sample the depth and normal visualization like the color.
				// The requirement is just to take an average of all the samples within the pixel
				// (so-called "box filtering"). Note that this starter code does not take an average,
				// it just assumes the first and only sample is the final color.

				// For extra credit, you can implement more sophisticated ones, such as "tent" and bicubic
				// "Mitchell-Netravali" filters. This requires you to implement the addSample()
				// function in the Film class and use it instead of directly setting pixel values in the image.

				// YOUR CODE HERE (R0)
				// If args.display_uv is true, we want to render a test UV image where the color of each pixel
				// is a simple function of its position in the image. The red component should linearly increase
				// from 0 to 1 with the x coordinate increasing from 0 to args.width. Likewise the green component
				// should linearly increase from 0 to 1 as the y coordinate increases from 0 to args.height. Since
				// our image is two-dimensional we can't map blue to a simple linear function and just set it to 1.

				//if (args.display_uv)
				//	sample_color = ...

				image->setVec4f(Vec2i(i,j), Vec4f(sample_color, 1));
				if (depth_image) {
					// YOUR CODE HERE (R2)
					// Here you should linearly map the t range [depth_min, depth_max] to the inverted range [1,0] for visualization
					// Note the inversion; closer objects should appear brighter.
					float f = 0.0f;
					depth_image->setVec4f(Vec2i(i, j), Vec4f(Vec3f(f), 1));
				}
				if (normals_image) {
					Vec3f normal = hit.normal;
					Vec3f col(fabs(normal[0]), fabs(normal[1]), fabs(normal[2]));
					col = col.clamp(Vec3f(0), Vec3f(1));
					normals_image->setVec4f( Vec2i( i, j ), Vec4f( col, 1 ) );
				}
			}
		}
		++lines_done;
	}

	// YOUR CODE HERE (EXTRA)
	// When you implement smarter filtering, you should normalize the filter weight
	// carried in the 4th channel.
	if (image) {
	}

	// And finally, save the images as PNG!
	if (image) {
		FW::File f(args.output_file.c_str(), FW::File::Create);
		exportLodePngImage(f, image.get());
	}
	if (depth_image) { 
		FW::File f(args.depth_file.c_str(), FW::File::Create);
		exportLodePngImage(f, depth_image.get());
	}
	if (normals_image) { 
		FW::File f(args.normals_file.c_str(), FW::File::Create);
		exportLodePngImage(f, normals_image.get());
	}

	return image->createGLTexture();
}
