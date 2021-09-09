#pragma once

#include "hit.hpp"
#include "ray.hpp"
#include "utility.hpp"

#include "gui/Image.hpp"
#include "io/File.hpp"
#include "io/ImageTargaIO.hpp"

#include <cassert>

class Light;

class Material
{
public:
	Material() {}
	Material(const FW::Vec3f& diffuse_color,
			const FW::Vec3f& reflective_color,
			const FW::Vec3f& transparent_color,
			float refraction_index,
			const char* texture_filename ) :
		diffuse_color_(diffuse_color),
		reflective_color_(reflective_color),
		transparent_color_(transparent_color),
		refraction_index_(refraction_index),
		texture_()
	{
		if (texture_filename)
			texture_.reset(FW::importImage(texture_filename));
	}
	virtual ~Material() {}

	virtual FW::Vec3f diffuse_color(const FW::Vec3f& point) const = 0;
	virtual FW::Vec3f reflective_color(const FW::Vec3f& point) const = 0;
	virtual FW::Vec3f transparent_color(const FW::Vec3f& point) const = 0;
	virtual float refraction_index(const FW::Vec3f& point) const = 0;

	// This function evaluates the light reflected at the point determined
	// by hit.t along the ray, in the direction of Ray, when lit from
	// light incident from dirToLight at the specified intensity.
	virtual FW::Vec3f shade(const Ray& ray, const Hit& hit, const FW::Vec3f& dir_to_light, const FW::Vec3f& incident_intensity, bool shade_back ) const = 0;

protected:
	FW::Vec3f diffuse_color_;
	FW::Vec3f reflective_color_;
	FW::Vec3f transparent_color_;
	float refraction_index_;

	std::unique_ptr<FW::Image> texture_;
};

// This class implements the Phong shading model.
class PhongMaterial : public Material
{
public:
	PhongMaterial(const FW::Vec3f& diffuse_color, const FW::Vec3f& specular_color, float exponent, const FW::Vec3f& reflective_color,
			const FW::Vec3f& transparent_color, float refraction_index, const char* texture_TGA_filename) :
		Material(diffuse_color, reflective_color, transparent_color, refraction_index, texture_TGA_filename),
		specular_color_(specular_color), exponent_(exponent)
	{}

	FW::Vec3f	diffuse_color(const FW::Vec3f&) const override { return diffuse_color_; }
	FW::Vec3f	reflective_color(const FW::Vec3f&) const override { return reflective_color_; }
	FW::Vec3f	transparent_color(const FW::Vec3f&) const { return transparent_color_; }
	float		refraction_index(const FW::Vec3f&) const override { return refraction_index_; }
	FW::Vec3f	specular_color() const { return specular_color_; }
	float		exponent() const { return exponent_; }

	// You need to fill in this implementation of this function.
	FW::Vec3f shade(const Ray& ray, const Hit& hit, const FW::Vec3f& dir_to_light, const FW::Vec3f& incident_intensity, bool shade_back) const override;

private:
	FW::Vec3f specular_color_;
	float exponent_;
};


class ProceduralMaterial : public Material
{
public:
	ProceduralMaterial(const FW::Mat4f& matrix, Material* m1, Material* m2) :
		matrix_(matrix), m1_(m1), m2_(m2)
	{ assert(m1 != nullptr && m2 != nullptr); }

	FW::Vec3f	diffuse_color(const FW::Vec3f& point) const override;
	FW::Vec3f	reflective_color(const FW::Vec3f& point) const override;
	FW::Vec3f	transparent_color(const FW::Vec3f& point) const override;
	float		refraction_index(const FW::Vec3f& point) const override;

	FW::Vec3f shade(const Ray& ray, const Hit& hit, const FW::Vec3f& dir_to_light, const FW::Vec3f& lightColor, bool shade_back) const override;
	virtual float interpolation(const FW::Vec3f& point) const = 0;

protected:
	FW::Mat4f matrix_;
	Material* m1_;
	Material* m2_;
};

class Checkerboard : public ProceduralMaterial
{
public:
	Checkerboard(const FW::Mat4f& matrix, Material* m1, Material* m2) :
		ProceduralMaterial(matrix, m1, m2)
	{}
	float interpolation(const FW::Vec3f& point) const override;
};
