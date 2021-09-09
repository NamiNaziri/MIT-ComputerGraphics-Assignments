#include "lights.hpp"

#include <cfloat>

using namespace FW;

DirectionalLight::DirectionalLight() :
	direction_(0, 0, 0),
	intensity_(1, 1, 1)
{}

DirectionalLight::DirectionalLight(const Vec3f& direction, const Vec3f& intensity) :
	direction_(direction.normalized()),	intensity_(intensity)
{}

void DirectionalLight::getIncidentIllumination(const Vec3f& p,
		Vec3f& dir_to_light, Vec3f& incident_intensity, float& distance) const {
	(void)p; // suppress compiler warning for unused parameter

	// YOUR CODE HERE (R4)
	// You should fill in the appropriate information about the "infinitely distant" light
	// for the renderer to use. See the comment in the base class for extra information.
}

PointLight::PointLight(const Vec3f& position, const Vec3f& intensity,
		float constantAttenuation,
		float linearAttenuation,
		float quadraticAttenuation) :
	position_(position),
	intensity_(intensity),
	constant_attenuation_(constantAttenuation),
	linear_attenuation_(linearAttenuation),
	quadratic_attenuation_(quadraticAttenuation)
{}

void PointLight::getIncidentIllumination(const Vec3f& p,
		Vec3f& dir_to_light, Vec3f& incident_intensity, float& distance) const {
	// YOUR CODE HERE (R4)
	// Evaluate the incident illumination at point p, accounting
	// for distance attenuation. Distance attenuation should be
	// computed using 1/(ar^2+br+c), with a, b, c given by the members
	// quadratic_attenuation_, linear_attenuation_ and constant_attenuation_.
	// To facilitate the implementation of Phong, do not take the
	// incident cosine into account here.
}
