#pragma once

#include "base/Math.hpp"

// Abstract base class for lights.
class Light
{
public:
	virtual ~Light() {}
	
	// This function evaluates, at the point p in the scene,
	// what direction the light is at, what its intensity
	// is at the shading point, and how far away the light is.
	// The distance is only used for shadow rays, actual attenuation
	// due to distance should be accounted for in incident_intensity.
	// To facilitate the implementation of Phong, do not take the
	// incident cosine into account here.
	// ----
	// NOTE: This function doesn't return anything. This function
	// uses "output parameters":
	// Instead of having the function return some values, the
	// function parameters are modified inside this function, and
	// the caller function can get the values from those objects.
	// The only input parameter for this function is p, and
	// dir_to_light, incident_intensity and distance are evaluated
	// in this function.
	virtual void getIncidentIllumination(const FW::Vec3f& p, FW::Vec3f& dir_to_light, FW::Vec3f& incident_intensity, float& distance) const = 0;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(const FW::Vec3f& direction, const FW::Vec3f& intensity);

	// You need to fill in the implementation.
	void getIncidentIllumination(const FW::Vec3f& p, FW::Vec3f& dir_to_light, FW::Vec3f& incident_intensity, float& distance) const override;
private:
	FW::Vec3f direction_;
	FW::Vec3f intensity_;
};

class PointLight : public Light
{
public:
	PointLight(const FW::Vec3f& position, const FW::Vec3f& intensity,
			float constant_attenuation,
			float linear_attenuation,
			float quadratic_attenuation);

	// You need to fill in the implementation.
	void getIncidentIllumination(const FW::Vec3f& p, FW::Vec3f& dir_to_light, FW::Vec3f& incident_intensity, float& distance) const override;

private:
	PointLight();

	FW::Vec3f position_;
	FW::Vec3f intensity_;

	float constant_attenuation_;
	float linear_attenuation_;
	float quadratic_attenuation_;
};
