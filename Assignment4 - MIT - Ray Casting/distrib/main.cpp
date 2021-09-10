#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "SceneParser.h"
#include "Image.h"
#include "Camera.h"
#include <string.h>

using namespace std;

float clampedDepth ( float depthInput, float depthMin , float depthMax);

#include "bitmap_image.hpp"
int main( int argc, char* argv[] )
{
	// Fill in your implementation here.

	// This loop loops over each of the input arguments.
	// argNum is initialized to 1 because the first
	// "argument" provided to the program is actually the
	// name of the executable (in our case, "a4").
	for( int argNum = 1; argNum < argc; ++argNum )
	{
	  std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
	}


	// First, parse the scene using SceneParser.
	// Then loop over each pixel in the image, shooting a ray
	// through that pixel and finding its intersection with
	// the scene.  Write the color at the intersection to that
	// pixel in your output image.
	SceneParser sceneParser(argv[2]);
	const int w = stoi(argv[4]);
	const int h = stoi(argv[5]);
	Image image(w, h);

	Group* group = sceneParser.getGroup();
	Camera* camera = sceneParser.getCamera();
	
	for(int i = 0 ; i < w ;i++)
	{
		for(int j = 0 ; j < h ; j++)
		{
			float jj = static_cast<float>(j);
			float ii = static_cast<float>(i);
			float ww = static_cast<float>(w);
			float hh = static_cast<float>(h);

			float x = (ii - (ww / 2)) / (ww / 2);
			float y = (jj - (hh / 2)) / (hh / 2);
			Vector2f point = Vector2f(x,y );
			Ray ray = camera->generateRay(point);
			Hit hit;
			Vector3f pixelColor;
			bool isHitted = group->intersect(ray, hit, camera->getTMin());
			if (isHitted)
			{
				pixelColor = Vector3f(1, 1, 1);
				image.SetPixel(i , j , pixelColor);
			}
			else
			{
				pixelColor = Vector3f(1, 0, 1);
				image.SetPixel(i , j, pixelColor);
			}
			
		}
	}

	image.SaveImage(argv[7]);


	return 0;
}

