#pragma once

#include <cassert>

#include "base/Math.hpp"
#include "gui/Image.hpp"

using namespace FW; // TODO: fix

class Filter;

// A helper class for super-sampling and smart filtering.

class Film
{
public:

	Film( Image* img, Filter* filter );
	~Film();

	// YOUR CODE HERE (EXTRA)
	// Implement this function to perform smarter filtering.
	void addSample( const Vec2f& pixelCoordinates, const Vec3f& sampleColor );

private:
	Image* m_image;
	Filter* m_filter;
};
