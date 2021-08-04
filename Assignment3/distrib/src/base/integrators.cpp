
#include "utility.hpp"
#include "particle_systems.hpp"
#include "integrators.hpp"

void eulerStep(ParticleSystem& ps, float step) {
	// YOUR CODE HERE (R1)
	// Implement an Euler integrator.

	State newState;


	State currentState = ps.state();
	std::vector<FW::Vec3f> f = ps.evalF(currentState);
	for (size_t i = 0; i < currentState.size(); i++)
	{

		newState.push_back(currentState[i] + step * f[i]);
	}

	ps.set_state(newState);
};

void trapezoidStep(ParticleSystem& ps, float step) {
	// YOUR CODE HERE (R3)
	// Implement a trapezoid integrator.
	const State currentState = ps.state();
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

	
}

void midpointStep(ParticleSystem& ps, float step) {
	const auto& x0 = ps.state();
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
}

void rk4Step(ParticleSystem& ps, float step) {
	// EXTRA: Implement the RK4 Runge-Kutta integrator.
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
