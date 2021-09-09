#pragma once

#include "hit.hpp"
#include "objects.hpp"
#include "ray.hpp"

struct Args;
class SceneParser;

struct RaySegment {
	RaySegment(FW::Vec3f origin, FW::Vec3f offset, FW::Vec3f normal_at_offset, FW::Vec3f color) : origin(origin), offset(offset), normal_at_offset(normal_at_offset), color(color) {}
	FW::Vec3f origin, offset, normal_at_offset, color;
};

class RayTracer
{
public:
	RayTracer(const SceneParser& scene, const Args& args, bool debug = false) :
		args_(args),
		scene_(scene),
		debug_trace(debug)
	{}

	// You need to fill in the implementation for this function.
	FW::Vec3f traceRay(Ray& ray, float tmin, int bounces, float refr_index, Hit& hit, FW::Vec3f debug_color) const;
	
	// For the debug visualisation: mutable means that we can modify it inside the traceRay method even though it is const.
	mutable std::vector < RaySegment > debug_rays;
private:
	RayTracer& operator=(const RayTracer&); // squelch compiler warning
	FW::Vec3f computeShadowColor(Ray& ray, float distanceToLight) const;

	bool debug_trace;

	const SceneParser&	scene_;
	const Args&			args_;
};
