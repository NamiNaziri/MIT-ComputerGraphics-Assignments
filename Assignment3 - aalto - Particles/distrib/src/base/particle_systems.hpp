#pragma once

#include <iostream>

#include "../framework/base/Math.hpp"

#include <vector>

// EXTRA: probably want to use Eigen for the implicit solver

//#include "../Eigen/SparseCore" // for the sparse matrices
//#include "../Eigen/SparseLU"   // LU decomposition for linear solves
//#include "../Eigen/StdVector"  // for interop with the State vectors

// less writing
// typedef Eigen::SparseMatrix < float, 0, Eigen::DenseIndex > SparseMatrix;
// typedef Eigen::SparseLU<SparseMatrix, Eigen::COLAMDOrdering<Eigen::DenseIndex>> SparseLU;

// Eigen supports a multitude of solvers, SparseLU is probably the best fit for us since our
// problems are sparse but not necessarily symmetric or positive definite.


typedef std::vector<FW::Vec3f> State;
typedef std::vector<FW::Vec3f> Points;
typedef std::vector<FW::Vec3f> Lines;

struct Spring {
	Spring() {}
	Spring(unsigned index1, unsigned index2, float spring_k, float rest_length) :
		i1(index1), i2(index2), k(spring_k), rlen(rest_length) {}
	unsigned i1, i2;
	float k, rlen;
};

class ParticleSystem {
public:
	virtual						~ParticleSystem() {};
	virtual State				evalF(const State&) const = 0;
#ifdef EIGEN_SPARSECORE_MODULE_H
	virtual	void				evalJ(const State&, SparseMatrix& result, bool initial) const = 0;
#endif
	virtual void				reset() = 0;
	virtual void				resetParticle(int particleIndex) {};
	const State&				state() { return current_state_; }
	virtual void						set_state(State s) { current_state_ = s; }
	bool						isTimeDependent() { return timeDependent; }
	const std::vector<float>&	time() { return time_; };
	void						set_time(std::vector<float> t) { time_ = t; }
	virtual Points				getPoints() { return Points(); }
	virtual Lines				getLines() { return Lines(); }
protected:
	State						current_state_;
	std::vector<float>			time_;
	bool						timeDependent;
};

class SimpleSystem : public ParticleSystem {
public:
	SimpleSystem() : radius_(0.5f) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

private:
	float					radius_;
};

class SpringSystem : public ParticleSystem {
public:
	SpringSystem() { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

private:
	Spring					spring_;
};

class PendulumSystem : public ParticleSystem {
public:
	PendulumSystem(unsigned n) : n_(n) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

	inline int						Pos(int particleIndex) const { return particleIndex * 2; }
	inline int						Vel(int particleIndex) const { return (particleIndex * 2) + 1; }

private:
	unsigned				n_;
	std::vector<Spring>		springs_;
};

class ClothSystem : public ParticleSystem {
public:
	ClothSystem(unsigned x, unsigned y) : x_(x), y_(y) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;
	void					set_state(State s) override;
	FW::Vec2i				getSize() { return FW::Vec2i(x_, y_); }
	void					setWindForceToggle(bool w) { wind_force_toggle_ = w; }
	void					setFrictionlessCollisionsToggle(bool fc) { frictionless_collisions_toggle_ = fc; }
	
	inline int						Pos(int x , int y) const
	{
		if(x < 0 || y < 0 || x >= x_ || y >= y_ )
		{
			std::cout << "index out of range" << std::endl;
		}
		return 2 * (x * y_ + y);
	}
	inline int						Vel(int x, int y) const
	{
		if (x < 0 || y < 0 || x >= x_ || y >= y_)
		{
			std::cout << "index out of range" << std::endl;
		}

		return (2 * (x * y_ + y)) + 1;
	}
	

private:
	unsigned				x_, y_;
	std::vector<Spring>		springs_;
	bool					wind_force_toggle_ = false;
	const int				wind_force_multiplier = 2;
	bool					frictionless_collisions_toggle_ = false;
};


class SprinklerSystem : public ParticleSystem
{
public:
	SprinklerSystem(unsigned n) : n_(n) { reset();}

	State					evalF(const State&) const override;
	void					reset() override;
	Points					getPoints() override;
	void					resetParticle(int particleIndex) override;
	

	inline int						Pos(int particleIndex) const { return particleIndex * 2; }
	inline int						Vel(int particleIndex) const { return (particleIndex * 2) + 1; }

private:
	unsigned				n_;
	const int				spread = 20;
	const int				gravityMultiplier = 40;
	
};

class FluidSystem : public ParticleSystem {
public:
							FluidSystem(unsigned n) : n_(n) { reset(); }
	State					evalF(const State&) const override;
#ifdef EIGEN_SPARSECORE_MODULE_H
	void					evalJ(const State&, SparseMatrix& result, bool initial) const override;
#endif
	void					reset() override;
	Points					getPoints() override;
	Lines					getLines() override;

private:
	unsigned				n_;
	std::vector<unsigned>	asd;
};
