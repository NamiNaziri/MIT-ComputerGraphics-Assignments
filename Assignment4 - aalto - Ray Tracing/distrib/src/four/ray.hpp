#pragma once

#include "base/Math.hpp"

#include <iostream>

struct Ray
{
	Ray(const FW::Vec3f& orig, const FW::Vec3f& dir) {
		origin = orig;
		direction = dir;
	}

	FW::Vec3f pointAtParameter(float t) const {
		return origin + direction * t;
	}

	FW::Vec3f origin;
	FW::Vec3f direction;
};

inline std::ostream& operator<<(std::ostream& os, const FW::Vec3f& v) {
	os << "[" << v.x << ", " << v.y << ", " << v.z << "]";
}

inline std::ostream& operator<<(std::ostream& os, const FW::Vec2f& v) {
	os << "[" << v.x << ", " << v.y << "]";
}

inline std::ostream& operator<<(std::ostream& os, const Ray& r) {
	os << "Ray <" << r.origin << ", " << r.direction << ">";
	return os;
}
