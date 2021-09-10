#ifndef SPHERE_H
#define SPHERE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;
///TODO:
///Implement functions and add more fields as necessary
class Sphere: public Object3D
{
public:
	Sphere(){ 
		//unit ball at the center
		this->center = Vector3f();
		this->radius = 1;
	}

	Sphere( Vector3f center , float radius , Material* material ):Object3D(material){
		this->center = center;
		this->radius = radius;
		this->material = material;
	}
	

	~Sphere(){}

	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		const float a = Vector3f::dot(r.getDirection(), r.getDirection());
		const float b = 2 * Vector3f::dot( r.getDirection(), r.getOrigin());
		const float c = Vector3f::dot(r.getOrigin(), r.getOrigin()) - (radius * radius);
		const float discriminant = sqrt (((b * b) - 4 * a * c) * ((b * b) - 4 * a * c));
		const float t1 = (-b + discriminant) / (2 * a);
		const float t2 = (-b - discriminant) / (2);
		float finalT;
		if(t1 > 0 && t2 > 0)
		{
			finalT = t1 < t2 ? t1 : t2;
		}
		else
		{
			//one of them is negative
			finalT = t1 > t2 ? t1 : t2;
		}
		

		const Vector3f intersectionPoint = r.getOrigin() + finalT * r.getDirection();
		const Vector3f intersectionNormal = (intersectionPoint - center).normalized();
		

		if(finalT > tmin && finalT < h.getT())
		{
			h.set(finalT, material, intersectionNormal);
			return true;
		}
		
		return false;
	}

protected:
	Vector3f center;
	float radius;
};


#endif
