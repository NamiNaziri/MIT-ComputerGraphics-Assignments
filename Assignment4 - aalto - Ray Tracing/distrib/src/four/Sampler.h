#pragma once

#include "args.hpp"

#include "base/Math.hpp"
#include "base/Random.hpp"

#include <cassert>
#include <cmath>

using namespace FW;

// for supersampling antialiasing

// Base class for sampler. Only supports one sample through the center of the pixel.
class Sampler
{
public:
	Sampler(int nSamples);
	virtual ~Sampler() {};
	virtual Vec2f getSamplePosition( int n ) = 0;

	// call this to get an instance of the proper subclass
	static Sampler* constructSampler( Args::SamplePatternType t, int numsamples );

protected:
	FW::Random	m_generator;
	int			m_nSamples;
};

// Regular subpixel grid sampler.
// Divide each pixel into sqrt(n)*sqrt(n) subpixels and return samples at their centers.
class RegularSampler : public Sampler
{
public:
	RegularSampler(int nSamples);
	Vec2f getSamplePosition(int n) override;

private:
	int m_dim;
};

// Regular subpixel grid sampler.
// Divide each pixel into sqrt(n)*sqrt(n) subpixels and return samples at random position inside the strata.
class JitteredSampler : public Sampler
{
public:
	JitteredSampler(int nSamples);
	Vec2f getSamplePosition(int n) override;

private:
	int m_dim;
};

// Uniform random sampling within the pixel. Each sample is chosen at random within the pixel.
class UniformSampler : public Sampler
{
public:
	UniformSampler(int nSamples);
	Vec2f getSamplePosition(int n) override;
};
