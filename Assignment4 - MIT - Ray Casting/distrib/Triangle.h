#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Object3D.h"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;
///TODO: implement this class.
///Add more fields as necessary,
///but do not remove hasTex, normals or texCoords
///they are filled in by other components
class Triangle: public Object3D
{
public:
	Triangle();
        ///@param a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m):Object3D(m){
          hasTex = false;
		  this->a = a;
		  this->b = b;
		  this->c = c;
	}

	virtual bool intersect( const Ray& ray,  Hit& hit , float tmin){

		
		Matrix3f betaMat = Matrix3f(a.x() - ray.getOrigin().x(), a.x() - c.x(), ray.getDirection().x(),
									a.y() - ray.getOrigin().y(), a.y() - c.y(), ray.getDirection().y(),
									a.z() - ray.getOrigin().z(), a.z() - c.z(), ray.getDirection().z()
								 );

		Matrix3f gamaMat = Matrix3f(a.x() - b.x(), a.x() - ray.getOrigin().x(), ray.getDirection().x(),
									a.y() - b.y(), a.y() - ray.getOrigin().y(), ray.getDirection().y(),
									a.z() - b.z(), a.z() - ray.getOrigin().z(), ray.getDirection().z()
								 );
		Matrix3f tMat =	   Matrix3f(a.x() - b.x(), a.x() - c.x(), a.x() - ray.getOrigin().x(),
									a.y() - b.y(), a.y() - c.y(), a.y() - ray.getOrigin().y(),
									a.z() - b.z(), a.z() - c.z(), a.z() - ray.getOrigin().z()
								 );

		Matrix3f A =	   Matrix3f(a.x() - b.x(), a.x() - c.x(), ray.getDirection().x(),
								     a.y() - b.y(), a.y() - c.y(), ray.getDirection().y(),
									 a.z() - b.z(), a.z() - c.z(), ray.getDirection().z()
								 );
		const float A_Determinant = A.determinant();

		const float beta = betaMat.determinant() / A_Determinant;
		const float gama = gamaMat.determinant() / A_Determinant;
		const float t =    tMat.determinant()    / A_Determinant;

		if(t > tmin && t < hit.getT() && beta >= 0 && gama >= 0 && beta + gama <= 1)
		{
			Vector3f normal = (1 - beta - gama) * normals[0] + beta * normals[1] + gama * normals[2];
			hit.set(t, this->material, normal.normalized());
			if(hasTex)
			{
				const Vector2f texCoord = (1 - beta - gama) * texCoords[0] + beta * texCoords[1] + gama * texCoords[2];
				hit.setTexCoord(texCoord);
			}
			return true;
		}
		
		return false;
	}
	bool hasTex;
	Vector3f normals[3];
	Vector2f texCoords[3];
	Vector2f UV;
protected:
	Vector3f a, b, c;
	
};

#endif //TRIANGLE_H
