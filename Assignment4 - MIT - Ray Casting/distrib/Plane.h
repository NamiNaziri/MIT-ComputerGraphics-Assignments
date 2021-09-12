#ifndef PLANE_H
#define PLANE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
using namespace std;
///TODO: Implement Plane representing an infinite plane
///choose your representation , add more fields and fill in the functions
class Plane: public Object3D
{
public:
	Plane(){}
	Plane( const Vector3f& normal , float d , Material* m):Object3D(m){
		this->normal = normal;
		this->d = d;
	}
	~Plane(){}
	virtual bool intersect( const Ray& r , Hit& h , float tmin){

		const float normalDotRayDirection = Vector3f::dot(normal, r.getDirection());

		if (abs( normalDotRayDirection ) < std::numeric_limits<float>::epsilon()) // ray is parallel to the plain
		{
			return false;
		}

		/*
		 * in the equation in slides d is the negate of d in here
		 * slide equation : P . n + d = 0
		 * Here :			P . n = d
		 */

		
		const float t = (-((-d) + Vector3f::dot(normal, r.getOrigin()))) / (normalDotRayDirection);
		
		if(t > tmin && t < h.getT())
		{
			h.set(t, this->material, normal );
			return true;
		}

		return false;
			
		
	}

protected:
	Vector3f normal;
	float d;
	
	
};
#endif //PLANE_H
		

