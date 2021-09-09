#pragma once

#include "base/Math.hpp"

class VecUtils
{
public:
	static FW::Mat4f rotate(const FW::Vec3f& axis, float angle)
	{
		FW::Mat3f R = FW::Mat3f::rotation(axis, angle);
		FW::Mat4f R4;
		R4.setIdentity();
		R4.setCol(0, FW::Vec4f(FW::Vec3f(R.getCol(0)), 0.0f));
		R4.setCol(1, FW::Vec4f(FW::Vec3f(R.getCol(1)), 0.0f));
		R4.setCol(2, FW::Vec4f(FW::Vec3f(R.getCol(2)), 0.0f));
		return R4;
	}

	// transforms a 3D point using a matrix, returning a 3D point
	static FW::Vec3f transformPoint(const FW::Mat4f& mat, const FW::Vec3f& point) {
		return (mat * FW::Vec4f(point, 1)).getXYZ();
	}

	// transform a 3D directino using a matrix, returning a direction
	// This function *does not* take the inverse tranpose for you.
	static FW::Vec3f transformDirection(const FW::Mat4f& mat, const FW::Vec3f& dir) {
		return (mat * FW::Vec4f(dir, 0)).getXYZ();
	}
};
