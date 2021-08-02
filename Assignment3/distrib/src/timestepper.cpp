#include "timestepper.h"

#include <cstdio>

void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
   //TODO: See handout 3.1
	auto newState = new std::vector<Vector3f>();


	std::vector<Vector3f> currentState = particleSystem->getState();
	std::vector<Vector3f> f = particleSystem->evalF(currentState);
	for(size_t i = 0 ; i < currentState.size() ; i++)
	{
		
		newState->push_back(currentState[i] + stepSize * f[i]);
	}
	
	particleSystem->setState(*newState);
}

void Trapezoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	auto newState = new std::vector<Vector3f>();

	std::vector<Vector3f> currentState = particleSystem->getState();
	std::vector<Vector3f> f0 = particleSystem->evalF(currentState);
	std::vector<Vector3f> newState0(currentState.size());
	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState0.push_back(currentState[i] + stepSize * f0[i]);
	}


	std::vector<Vector3f> f1 = particleSystem->evalF(newState0);

	for (size_t i = 0; i < currentState.size(); i++)
	{
		newState->push_back(currentState[i] + (stepSize / 2) * (f0[i] + f1[i]));
	}

	particleSystem->setState(*newState);
}


void RK4::takeStep(ParticleSystem* particleSystem, float stepSize)
{
}

