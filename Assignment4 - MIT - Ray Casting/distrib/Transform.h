#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
  Transform(){}
 Transform( const Matrix4f& m, Object3D* obj ):o(obj),transformMat(m){
    
  }
  ~Transform(){
  }
  virtual bool intersect(const Ray& r, Hit& h, float tmin) {

      Vector4f newOrgin = transformMat.inverse() * Vector4f(r.getOrigin(), 1);
      Vector4f newDirection = transformMat.inverse() * Vector4f(r.getDirection(), 0);
      Ray newRay = Ray(newOrgin.xyz(), newDirection.xyz());
      bool isIntersected = o->intersect(newRay, h, tmin);
      if(isIntersected)
      {
          Vector4f newNormal = transformMat.inverse().transposed() * Vector4f(h.getNormal(), 0);
          h.set(h.getT(), h.getMaterial(), newNormal.xyz().normalized());
      }
      return isIntersected;
      
  	
  }

 protected:
	Object3D* o; //un-transformed object
	Matrix4f transformMat;
};

#endif //TRANSFORM_H
