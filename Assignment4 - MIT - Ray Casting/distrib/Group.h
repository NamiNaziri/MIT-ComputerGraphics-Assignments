#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

  Group(){
	  objectsNum = 0;
  }
	
  Group( int num_objects ){
	objectsNum = num_objects;
	ObjectList = new Object3D*[objectsNum];
  	
  }

  ~Group(){

	for(int i = 0 ; i < objectsNum; i++)
  	{
		delete ObjectList[i];
  	}
  	
	delete ObjectList;
  }

  virtual bool intersect( const Ray& r , Hit& h , float tmin ) {
  		for(int i = 0 ; i < objectsNum ; i++)
  		{
			ObjectList[i]->intersect(r,h, tmin);
  		}
		return false;
   }
	
  void addObject( int index , Object3D* obj ){
	  if(index < 0 || index >= objectsNum)
	  {
		  cout << __FUNCTION__ << "index Out of bounds" << endl;
	  }
	  ObjectList[index] = obj;
  	
  }

  int getGroupSize(){ 
	  return objectsNum;
  }

 private:
	int objectsNum;
	Object3D** ObjectList;
};

#endif
	
