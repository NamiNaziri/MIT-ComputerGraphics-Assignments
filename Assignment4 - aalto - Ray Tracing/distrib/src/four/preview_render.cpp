#include "objects.hpp"

#include "hit.hpp"
#include "VecUtils.h"

#include <cassert>

using namespace std;
using namespace FW;


void Group::preview_render(GLContext* gl, const FW::Mat4f& objectToCamera, const FW::Mat4f& cameraToClip) const {
	if (preview_mesh)
		preview_mesh->draw(gl, objectToCamera, cameraToClip);
	else
		for (auto& o : objects_)
			o->preview_render(gl, objectToCamera, cameraToClip);
}

void Transform::preview_render(GLContext* gl, const FW::Mat4f& objectToCamera, const FW::Mat4f& cameraToClip) const {
	object_->preview_render(gl, objectToCamera*matrix_, cameraToClip);
}

void Plane::preview_render(GLContext* gl, const FW::Mat4f& objectToCamera, const FW::Mat4f& cameraToClip) const {
	Mat4f matrix;
	auto n = normal_.normalized(); Vec3f b, c;
	if (cross(n, Vec3f(1.0f, .0f, .0f)).length() > .0001f)
		b = cross(n, Vec3f(1.0f, .0f, .0f));
	else
		b = cross(n, Vec3f(.0f, 1.0f, .0f));
	c = cross(b, n);
	
	matrix.setCol(0, Vec4f(c, .0f));
	matrix.setCol(1, Vec4f(n, .0f));
	matrix.setCol(2, Vec4f(b, .0f));
	matrix.setCol(3, Vec4f(offset_*normal_, 1.0f));

	preview_mesh->draw(gl, objectToCamera*matrix, cameraToClip);
}

void Sphere::preview_render(GLContext* gl, const FW::Mat4f& objectToCamera, const FW::Mat4f& cameraToClip) const {
	Mat4f matrix;
	matrix.setCol(0, Vec4f(radius_, .0f, .0f, .0f));
	matrix.setCol(1, Vec4f(.0f, radius_, .0f, .0f));
	matrix.setCol(2, Vec4f(.0f, .0f, radius_, .0f));
	matrix.setCol(3, Vec4f(center_, 1.0f));

	preview_mesh->draw(gl, objectToCamera*matrix, cameraToClip);
}

void Box::preview_render(GLContext* gl, const FW::Mat4f& objectToCamera, const FW::Mat4f& cameraToClip) const {
	Mat4f matrix;
	matrix.setCol(0, Vec4f(max_.x-min_.x, .0f, .0f, .0f));
	matrix.setCol(1, Vec4f(.0f, max_.y-min_.y, .0f, .0f));
	matrix.setCol(2, Vec4f(.0f, .0f, max_.z-min_.z, .0f));
	matrix.setCol(3, Vec4f(min_, 1.0f));

	preview_mesh->draw(gl, objectToCamera*matrix, cameraToClip);
}

void Triangle::preview_render(GLContext* gl, const FW::Mat4f& objectToCamera, const FW::Mat4f& cameraToClip) const {
	Mat4f matrix;
	matrix.setCol(0, Vec4f(vertices_[2] - vertices_[0], .0f));
	matrix.setCol(1, Vec4f(.0f));
	matrix.setCol(2, Vec4f(vertices_[0] - vertices_[1], .0f));
	matrix.setCol(3, Vec4f(vertices_[0], 1.0f));

	preview_mesh->draw(gl, objectToCamera*matrix, cameraToClip);
}
