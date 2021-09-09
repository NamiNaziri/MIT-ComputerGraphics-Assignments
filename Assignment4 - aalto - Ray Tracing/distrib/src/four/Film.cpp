#include "Film.h"

#include <cmath>

// framework includes
#include "io/File.hpp"
#include "io/ImageLodePngIO.hpp"

// other
#include "Filter.h"

Film::Film( Image* img, Filter* filter )
{ 
	m_image = img;
	m_filter = filter;
}

Film::~Film()
{
}

/*** YOUR CODE HERE (EXTRA)
     The idea is that each incoming sample is turned from an infinitesimal point-like
	 thing into a function defined on a continuous domain by centering a filter around
	 the sample position. Then, it's a simple matter to evaluate the filter at the centers
	 of the affected pixels, and add the color, weighted by the filter into the touched
	 pixels. A neat trick is to carry the accumulated filter weight along in the fourth
	 channel, so that you can divide by it in the end, and not worry about normalizing
	 the filters.
**/
void Film::addSample( const Vec2f& samplePosition, const Vec3f& sampleColor )
{
}
