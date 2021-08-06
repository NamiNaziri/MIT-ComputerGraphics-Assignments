
#include "utility.hpp"
#include "particle_systems.hpp"
#include "integrators.hpp"
#include "base/Random.hpp"


float rand_uniform(float low, float hi) {
	float abs = hi - low;
	float f = (float)rand() / RAND_MAX;
	f *= abs;
	f += low;
	return f;
}


void CheckTimer(ParticleSystem& ps, std::vector<float>& currentTime,float step, int particleIndex)
{

	FW::Random r;
	if (ps.isTimeDependent() )
	{
		if (currentTime[particleIndex] < rand_uniform(0.1, 0.5))
		{
			currentTime[particleIndex] += step;
		}
		else
		{
			currentTime[particleIndex] = 0;
			ps.resetParticle(particleIndex);
		}

	}
}

void eulerStep(ParticleSystem& ps, float step) {
	// YOUR CODE HERE (R1)
	// Implement an Euler integrator.

	
	
	


	State currentState = ps.state();
	std::vector<float> currentTime = ps.time();
	State newState;
	
	std::vector<FW::Vec3f> f = ps.evalF(currentState);
	for (size_t i = 0; i < currentState.size(); i++)
	{

		newState.push_back(currentState[i] + step * f[i]);
	}

	ps.set_state(newState);

	//first change current state with set_state function and then check timer!
	for (size_t i = 0; i < currentState.size(); i++)
	{
		if (i % 2 == 0 && currentTime.size() > 0)
		{
			CheckTimer(ps, currentTime, step, i / 2);
		}
	}

	
	ps.set_time(currentTime);
};

void trapezoidStep(ParticleSystem& ps, float step) {
	// YOUR CODE HERE (R3)
	// Implement a trapezoid integrator.
	const State currentState = ps.state();
	std::vector<float> currentTime = ps.time();
	State newState(currentState.size());

	
	const std::vector<FW::Vec3f> f0 = ps.evalF(currentState);
	State newState0(currentState.size());
	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState0[i]=currentState[i] + step * f0[i];
	}


	const std::vector<FW::Vec3f> f1 = ps.evalF(newState0);

	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState[i] = currentState[i] + (step / 2) * (f0[i] + f1[i]);
	}

	ps.set_state(newState);
	//first change current state with set_state function and then check timer!
	for (size_t i = 0; i < currentState.size(); i++)
	{
		if (i % 2 == 0 && currentTime.size() > 0)
		{
			CheckTimer(ps, currentTime, step, i / 2);
		}
	}
	ps.set_time(currentTime);
	
}

void midpointStep(ParticleSystem& ps, float step) {
	const auto& x0 = ps.state();
	std::vector<float> currentTime = ps.time();
	
	auto n = x0.size();
	auto f0 = ps.evalF(x0);
	auto xm = State(n), x1 = State(n);
	for (auto i = 0u; i < n; ++i) {
		xm[i] = x0[i] + (0.5f * step) * f0[i];
	}
	auto fm = ps.evalF(xm);
	for (auto i = 0u; i < n; ++i) {
		x1[i] = x0[i] + step * fm[i];
	}
	ps.set_state(x1);

	//first change current state with set_state function and then check timer!
	for (auto i = 0u; i < n; ++i)
	{
		if (i % 2 == 0 && currentTime.size() > 0)
		{
			CheckTimer(ps, currentTime, step, i / 2);
		}
	}
	ps.set_time(currentTime);
}

void rk4Step(ParticleSystem& ps, float step) {
	// EXTRA: Implement the RK4 Runge-Kutta integrator.

	const State currentState = ps.state();
	std::vector<float> currentTime = ps.time();
	State newState(currentState.size());
	


	const std::vector<FW::Vec3f> f0 = ps.evalF(currentState);
	State newState0(currentState.size());
	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState0[i] = currentState[i] + (step / 2) * f0[i];
	}


	const std::vector<FW::Vec3f> f1 = ps.evalF(newState0);

	State newState1(currentState.size());
	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState1[i] = currentState[i] + (step / 2) * f1[i];
	}


	const std::vector<FW::Vec3f> f2 = ps.evalF(newState1); 

	State newState2(currentState.size());
	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState2[i] = currentState[i] + (step) * f2[i];
	}


	const std::vector<FW::Vec3f> f3 = ps.evalF(newState2); 


	
	////////////////

	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState[i] = currentState[i] + (1.f/6.f) * (step) * (f0[i] + f1[i] * 2 + f2[i] * 2 + f3[i] * 2);
	}

	ps.set_state(newState);
	
	for (size_t i = 0; i < currentState.size(); i++)
	{
		if (i % 2 == 0 && currentTime.size() > 0)
		{
			CheckTimer(ps, currentTime, step, i / 2);
		}
	}
	
	ps.set_time(currentTime);
}

#ifdef EIGEN_SPARSECORE_MODULE_H

void implicit_euler_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit Euler integrator. (Note that the related formula on page 134 on the lecture slides is missing a 'h'; the formula should be (I-h*Jf(Yi))DY=-F(Yi))
}

void implicit_midpoint_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial) {
	// EXTRA: Implement the implicit midpoint integrator.
}

void crank_nicolson_step(ParticleSystem & ps, float step, SparseMatrix & J, SparseLU & solver, bool initial) {
		// EXTRA: Implement the crank-nicolson integrator.
}
#endif
