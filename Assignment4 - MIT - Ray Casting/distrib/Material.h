#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
 Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0):
  diffuseColor( d_color),specularColor(s_color), shininess(s)
  {
        	
  }

  virtual ~Material()
    {

    }

  virtual Vector3f getDiffuseColor() const 
  { 
    return  diffuseColor;
  }
    

  Vector3f Shade( const Ray& ray, const Hit& hit,
                  const Vector3f& dirToLight, const Vector3f& lightColor ) {


      Vector3f Kd = diffuseColor;
 	if(t.valid())
 	{
 		if(hit.hasTex)
 		{
            Kd = t(hit.texCoord.x(), hit.texCoord.y());
            diffuseColor = Kd;
 		}
 	}

 	//Diffuse shading
      Vector3f DiffuseColor = lightColor * max( Vector3f::dot(hit.getNormal(), dirToLight), 0.f) * Kd;
      
 	//Specular 
      const Vector3f ReflectedRay = (ray.getDirection()) + 2 * Vector3f::dot(-ray.getDirection(), hit.getNormal()) * hit.getNormal();

      float Cs = powf(max(Vector3f::dot(dirToLight, ReflectedRay),0.f), shininess) ;
      const Vector3f SpecularColor = lightColor * Cs * specularColor;
 	
    return DiffuseColor + SpecularColor;
		
  }

  void loadTexture(const char * filename){
    t.load(filename);
  }
 protected:
  Vector3f diffuseColor;
  Vector3f specularColor;
  float shininess;
  Texture t;
};



#endif // MATERIAL_H
