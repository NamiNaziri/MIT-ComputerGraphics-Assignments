#include "particle_systems.hpp"
#include "base/Random.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>


using namespace std;
using namespace FW;

namespace {

	inline Vec3f fGravity(float mass) {
		return Vec3f(0, -9.8f * mass, 0);
	}

	// force acting on particle at pos1 due to spring attached to pos2 at the other end
	inline Vec3f fSpring(const Vec3f& pos1, const Vec3f& pos2, float k, float rest_length) {
		// YOUR CODE HERE (R2)
		const FW::Vec3f d = pos2 - pos1;
		return -k * (d.length() - rest_length) * (d / d.length());
	}

	// force acting on particle at pos1 due to spring attached to pos2 at the other end
	
	inline Vec3f fDrag(const Vec3f& v, float k) {
		// YOUR CODE HERE (R2)
		return -k * v;
	}

	float rand_uniform(float low, float hi) {
		
		std::random_device rnd_device;
		std::mt19937 rnd_engine(rnd_device());
		std::uniform_real_distribution<float> distribution(low, hi);
		return distribution(rnd_engine);
	}

	inline Vec3f fWind(const int multiplier)
	{
		
		return FW::Vec3f(rand_uniform(-1, 1) * multiplier, rand_uniform(-1.2, 1) * multiplier, rand_uniform(-0.5, 1) * multiplier);
	}


} // namespace

void SimpleSystem::reset() {
	timeDependent = false;
	
	current_state_ = State(1, Vec3f(0, radius_, 0));
}

State SimpleSystem::evalF(const State& state) const {
	State f(1, Vec3f(-state[0].y, state[0].x, 0));
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H
// using the implicit Euler method, the simple system should converge towards origin -- as opposed to the explicit Euler, which diverges outwards from the origin.
void SimpleSystem::evalJ(const State&, SparseMatrix& result, bool initial) const {
	if (initial) {
		result.coeffRef(1, 0) = 1.0f;
		result.coeffRef(0, 1) = -1.0f;
	}
}
#endif

Points SimpleSystem::getPoints() {
	return Points(1, current_state_[0]);
}

Lines SimpleSystem::getLines() {
	static const auto n_lines = 50u;
	auto l = Lines(n_lines * 2);
	const auto angle_incr = 2 * FW_PI / n_lines;
	for (auto i = 0u; i < n_lines; ++i) {
		l[2 * i] = l[2 * i + 1] =
			Vec3f(radius_ * FW::sin(angle_incr * i), radius_ * FW::cos(angle_incr * i), 0);
	}
	rotate(l.begin(), l.begin() + 1, l.end());
	return l;
}

void SpringSystem::reset() {
	
	const auto start_pos = Vec3f(0.1f, -0.5f, 0.0f);
	const auto spring_k = 30.0f;
	const auto rest_length = 0.5f;
	timeDependent = false;
	current_state_ = State(4);
	// YOUR CODE HERE (R2)
	// Set the initial state for a particle system with one particle fixed
	// at origin and another particle hanging off the first one with a spring.
	// Place the second particle initially at start_pos.
	
	spring_ = Spring(0, 1, spring_k, rest_length);
	current_state_[0] = FW::Vec3f();
	current_state_[1] = FW::Vec3f();
	current_state_[2] = start_pos;
	current_state_[3] = FW::Vec3f();
}

State SpringSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	State f(4);
	// YOUR CODE HERE (R2)
	// Return a derivative for the system as if it was in state "state".
	// You can use the fGravity, fDrag and fSpring helper functions for the forces.
	f[0] = FW::Vec3f();
	f[1] = FW::Vec3f();
	f[2] = state[3];
	f[3] = (fGravity(mass) + fDrag(state[3],drag_k) + fSpring(state[0],state[2],30.f,0.5)) / mass;
	
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

// This is a very useful read for the Jacobians of the spring forces. It deals with spring damping as well, we don't do that -- our drag is simply a linear damping of velocity (that results in some constants in the Jacobian).
// http://blog.mmacklin.com/2012/05/04/implicitsprings/

void SpringSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.5f;
	const auto mass = 1.0f;
	// EXTRA: Evaluate the Jacobian into the 'result' matrix here. Only the free end of the spring should have any nonzero values related to it.
}
#endif

Points SpringSystem::getPoints() {
	auto p = Points(2);
	p[0] = current_state_[0]; p[1] = current_state_[2];
	return p;
}

Lines SpringSystem::getLines() {
	auto l = Lines(2);
	l[0] = current_state_[0]; l[1] = current_state_[2];
	return l;
}

void PendulumSystem::reset() {
	const auto spring_k = 1000.0f;
	
	const auto start_point = Vec3f(0);
	const auto end_point = Vec3f(0.05, -1.5, 0);
	timeDependent = false;
	current_state_ = State(2 * n_);
	springs_.clear();
	// YOUR CODE HERE (R4)
	// Set the initial state for a pendulum system with n_ particles
	// connected with springs into a chain from start_point to end_point with uniform intervals.
	// The rest length of each spring is its length in this initial configuration.

	const FW::Vec3f distance = end_point - start_point;
	float length = distance.length() / n_;
	for(unsigned i = 0 ; i < n_ ; i ++)
	{
		current_state_[Pos(i)] = start_point + ((distance / n_) * i) ;
		current_state_[Vel(i)] = FW::Vec3f();
		if(i != n_-1)
		{
			springs_.emplace_back(i, i + 1, spring_k, distance.length() / n_);
		}
		
	}
}

State PendulumSystem::evalF(const State& state) const {
	const auto drag_k = 0.5f;
	const auto mass = 0.5f;
	auto f = State(2 * n_);
	// YOUR CODE HERE (R4)
	// As in R2, return a derivative of the system state "state".

	
	
	for(unsigned i = 1 ; i < n_ ; i++)
	{
		f[Pos(i)] = state[Vel(i)];
		f[Vel(i)] = (fGravity(mass) + fDrag(state[Vel(i)], drag_k)) / mass;
		//f[3] = (fGravity(mass) + fDrag(state[3], drag_k) + fSpring(state[0], state[2], 30.f, 0.5)) / mass;

	}
	

	for(auto s : springs_)
	{
		const Vec3f& pos1 = state[Pos(s.i1)];
		const Vec3f& pos2 = state[Pos(s.i2)];
		f[Vel(s.i2)] += (fSpring(pos1,pos2,s.k,s.rlen )/ mass);
		f[Vel(s.i1)] += (-fSpring(pos1, pos2, s.k, s.rlen) / mass);
	}


	//First point is fixed.
	f[0] = FW::Vec3f();
	f[1] = FW::Vec3f();
	
	return f;
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void PendulumSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {

	const auto drag_k = 0.5f;
	const auto mass = 0.5f;

	// EXTRA: Evaluate the Jacobian here. Each spring has an effect on four blocks of the matrix -- both of the positions of the endpoints will have an effect on both of the velocities of the endpoints.
}
#endif


Points PendulumSystem::getPoints() {
	auto p = Points(n_);
	for (auto i = 0u; i < n_; ++i) {
		p[i] = current_state_[i * 2];
	}
	return p;
}

Lines PendulumSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(current_state_[2 * s.i1]);
		l.push_back(current_state_[2 * s.i2]);
	}
	return l;
}

void ClothSystem::reset() {
	const auto spring_k = 300.0f;
	const auto width = 2.f, height = 2.f; // width and height of the whole grid
	timeDependent = false;
	current_state_ = State(2 * x_*y_);
	springs_.clear();
	// YOUR CODE HERE (R5)
	// Construct a particle system with a x_ * y_ grid of particles,
	// connected with a variety of springs as described in the handout:
	// structural springs, shear springs and flex springs.
	float structuralRestVertical = height / x_ ;
	float structuralRestHorizontal = width / y_ ;
	
	float shearRest = sqrtf((structuralRestVertical * structuralRestVertical) + (structuralRestHorizontal * structuralRestHorizontal)) ;
	for (int i = 0 ; i < x_ ; i++)
	{
		for(int j = 0 ; j < y_ ; j++)
		{
			FW::Vec3f Position = FW::Vec3f((j * width / y_) - (width / 2), 0, (-i * height / x_) + (height / 2));
			current_state_[Pos(i, j)] = Position;
			current_state_[Vel(i, j)] = FW::Vec3f();
			
			// structural springs

			if (i + 1 < x_)
			{

				springs_.emplace_back(Pos(i, j) / 2, Pos(i + 1, j) / 2, spring_k, structuralRestVertical);

			}
			if(j + 1 < y_)
			{
				springs_.emplace_back(Pos(i, j) / 2, Pos(i , j + 1) / 2, spring_k, structuralRestHorizontal);
			}


			//shear springs

			if( i + 1 < x_ && j + 1 < y_)
			{
				springs_.emplace_back(Pos(i, j) / 2, Pos(i + 1, j + 1) / 2, spring_k, shearRest);
			}

			if (i - 1 >= 0 && j + 1 < y_)
			{
				springs_.emplace_back(Pos(i, j) / 2, Pos(i - 1, j + 1) / 2, spring_k, shearRest);
			}
			
			//flex springs
			
			if (i + 2 < x_)
			{

				springs_.emplace_back(Pos(i, j) / 2, Pos(i + 2, j) / 2, spring_k, 2 * structuralRestVertical);

			}
			if (j + 2 < y_)
			{
				springs_.emplace_back(Pos(i, j) / 2, Pos(i, j + 2) / 2, spring_k, 2 * structuralRestHorizontal);
			}
			
			
		}
	}
	
}

State ClothSystem::evalF(const State& state) const {
	const auto drag_k = 0.08f;
	const auto n = x_ * y_;
	static const auto mass = 0.025f;
	auto f = State(2 * n);
	// YOUR CODE HERE (R5)
	// This will be much like in R2 and R4.


	
	const Vec3f windForce = fWind(wind_force_multiplier);
	for (int i = 0; i < x_; i++)
	{
		for (int j = 0; j < y_; j++)
		{
			f[Pos(i, j)] = state[Vel(i, j)];
			f[Vel(i,j)] = (fGravity(mass) + fDrag(state[Vel(i,j)], drag_k)) / mass;
			if(wind_force_toggle_)
			{
				
				f[Vel(i, j)] += windForce / mass;
			}
		}
	}

	for (auto s : springs_)
	{
		const Vec3f& pos1 = state[s.i1 * 2];
		const Vec3f& pos2 = state[s.i2 * 2];
		f[(s.i2 * 2) + 1] += (+fSpring(pos1, pos2, s.k, s.rlen) / mass);
		f[(s.i1 * 2) + 1] += (-fSpring(pos1, pos2, s.k, s.rlen) / mass);
	}


	
	

	f[0] = FW::Vec3f();
	f[1] = FW::Vec3f();
	
	f[Pos(0, y_ - 1)] = FW::Vec3f();
	f[Vel(0, y_ - 1)] = FW::Vec3f();
	return f;
}



#ifdef EIGEN_SPARSECORE_MODULE_H

void ClothSystem::evalJ(const State& state, SparseMatrix& result, bool initial) const {
	const auto drag_k = 0.08f;
	static const auto mass = 0.025f;

	// EXTRA: Evaluate the Jacobian here. The code is more or less the same as for the pendulum.
}

#endif

Points ClothSystem::getPoints() {
	auto n = x_ * y_;
	auto p = Points(n);
	for (auto i = 0u; i < n; ++i) {
		p[i] = current_state_[2 * i];
	}
	return p;
}

Lines ClothSystem::getLines() {
	auto l = Lines();
	for (const auto& s : springs_) {
		l.push_back(current_state_[2 * s.i1]);
		l.push_back(current_state_[2 * s.i2]);
	}
	return l;
}

void ClothSystem::set_state(State s)
{
	//Frictionless collisions
	//sphere with loc(0,-2,0) and r = 3

	if(frictionless_collisions_toggle_)
	{
		const FW::Vec3f SphereLocation = FW::Vec3f(0, -1.5, 0);
		const int r = 1;
		for (int i = 0; i < x_; i++)
		{
			for (int j = 0; j < y_; j++)
			{
				const FW::Vec3f pos = s[Pos(i, j)];
				if ((pos - SphereLocation).lenSqr() < r * r)
				{
					//cout << "je" << endl;
				FW:Vec3f direction = (pos - SphereLocation).normalized();
					FW::Vec3f newLocation = direction * r;
					s[Pos(i, j)] = newLocation + SphereLocation;
					s[Vel(i, j)] = FW::Vec3f();
				}

			}
		}
	}
	
	

	current_state_ = s;
}

State SprinklerSystem::evalF(const State& state) const
{
	const auto mass = 3.0f;
	auto f = State(2 * n_);
	

	for (unsigned i = 0; i < n_; i++)
	{
		f[Pos(i)] = state[Vel(i)];
		f[Vel(i)] = (fGravity(mass) * gravityMultiplier) / mass;
	}

	return f;
}

void SprinklerSystem::reset()
{
	Random r;
	timeDependent = true;
	time_ = std::vector<float>(n_);
	current_state_ = State(2 * n_);
	for(size_t i = 0 ; i < n_ ; i++)
	{
		current_state_[Pos(i)] = Vec3f(0,0,0);
		current_state_[Vel(i)] = Vec3f(rand_uniform(-1, 1) * spread /2 , rand_uniform(0.2, 1) * spread * 2 , rand_uniform(-1, 1) * spread / 2) ;
		time_[i] = 0;
	}
}

Points SprinklerSystem::getPoints()
{
	auto p = Points(n_);
	for (auto i = 0u; i < n_; ++i) {
		p[i] = current_state_[i * 2];
	}
	return p;
}

void SprinklerSystem::resetParticle(int particleIndex)
{
	Random r;
	current_state_[Pos(particleIndex)] = Vec3f(0, 0, 0);
	current_state_[Vel(particleIndex)] = Vec3f(rand_uniform(-1, 1) * spread , rand_uniform(0.2, 1) * spread * 2, rand_uniform(-1, 1) * spread / 2);

}

State FluidSystem::evalF(const State&) const {
	return State();
}

