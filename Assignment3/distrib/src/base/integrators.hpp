#pragma once

class ParticleSystem;

void eulerStep(ParticleSystem& ps, float step);

void trapezoidStep(ParticleSystem& ps, float step);

void midpointStep(ParticleSystem& ps, float step);

void rk4Step(ParticleSystem& ps, float step);

#ifdef EIGEN_SPARSECORE_MODULE_H

// These enable us to pass in the Jacobian and solver in order to save some state and avoid memory reallocations.
void implicit_euler_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial);
void implicit_midpoint_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial);
void crank_nicolson_step(ParticleSystem& ps, float step, SparseMatrix& J, SparseLU& solver, bool initial);
#endif
