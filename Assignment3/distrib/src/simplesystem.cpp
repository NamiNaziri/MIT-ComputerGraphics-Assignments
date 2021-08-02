#include "simplesystem.h"

#include "camera.h"
#include "vertexrecorder.h"


SimpleSystem::SimpleSystem()
{
    // TODO 3.2 initialize the simple system
    Vector3f derivatives(1,0,0);
    m_vVecState.push_back(derivatives);

    Vector3f derivatives2(1, 2, 0);
    m_vVecState.push_back(derivatives2);
}

std::vector<Vector3f> SimpleSystem::evalF(std::vector<Vector3f> state)
{
    std::vector<Vector3f> f;


    for(size_t i = 0 ; i < state.size() ; i++)
    {
        Vector3f derivatives;
        derivatives.x() = state[i].y() * -1;
        derivatives.y() = state[i].x();
        derivatives.z() = 0;

        f.push_back(derivatives);
    	
    }

    return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw(GLProgram& gl)
{

    // TODO 3.2: draw the particle. 
    //           we provide code that draws a static sphere.
    //           you should replace it with your own
    //           drawing code.
    //           In this assignment, you must manage two
    //           kinds of uniforms before you draw
    //            1. Update material uniforms (color)
    //            2. Update transform uniforms
    //           GLProgram is a helper object that has
    //           methods to set the uniform state.

	for(auto elemnt: m_vVecState)
	{
        const Vector3f PARTICLE_COLOR(0.4f, 0.7f, 1.0f);
        gl.updateMaterial(PARTICLE_COLOR);
        Vector3f pos(elemnt); //YOUR PARTICLE POSITION
        gl.updateModelMatrix(Matrix4f::translation(pos));
        drawSphere(0.075f, 10, 10);
	}

    
}
