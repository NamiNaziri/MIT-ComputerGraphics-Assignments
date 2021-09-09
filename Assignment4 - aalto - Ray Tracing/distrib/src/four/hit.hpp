#pragma once

#include "ray.hpp"

#include "base/Math.hpp"

class Material;

struct Hit
{
public:
	Hit() : material(nullptr), t(FLT_MAX) {}
	Hit(float t_max) : material(nullptr), t(t_max) {} 
	Hit(const Hit& h) { 
		t = h.t;
		material = h.material; 
		normal = h.normal;
	}

	void set(float tnew, Material* m, const FW::Vec3f& n) {
		t = tnew;
		material = m;
		normal = n;
	}

	float		t;			// closest hit found so far
	Material*	material;
	FW::Vec3f	normal;
};

inline std::ostream& operator<<(std::ostream &os, const Hit& h) {
	os << "Hit <" << h.t << ", " << h.normal << ">";
	return os;
}
