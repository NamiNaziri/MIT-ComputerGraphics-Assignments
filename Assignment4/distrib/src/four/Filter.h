#pragma once

#include "Film.h"
#include "args.hpp"

class Filter
{
public:

    Filter();
    virtual ~Filter();

    virtual float getSupportRadius() const = 0;

	// assuming the filter is centered at the origin, what is the weight at (x,y)?
    virtual float getWeight( const Vec2f& p ) const = 0;

	// call this to instantiate the proper subclass
	static Filter* constructFilter( Args::ReconstructionFilterType t, float radius );

};

class BoxFilter : public Filter
{
public:
    // CONSTRUCTOR & DESTRUCTOR
    BoxFilter( float _radius );
	virtual ~BoxFilter() {}

    float getSupportRadius() const;
    float getWeight( const Vec2f& p ) const;

private:
    float radius;
};

class TentFilter : public Filter
{
public:

    TentFilter( float _radius );
	virtual ~TentFilter() {}

    float getSupportRadius() const;
    float getWeight( const Vec2f& p ) const;

private:
    float radius;
};

class GaussianFilter : public Filter
{
public:

    GaussianFilter( float _sigma );
	virtual ~GaussianFilter() {}

    float getSupportRadius() const;
    float getWeight( const Vec2f& p ) const;

private: 
    float sigma;
    float radius;
};

