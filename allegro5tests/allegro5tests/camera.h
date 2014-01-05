//camera.h
//declarations of camera class including buidling of viewport

#ifndef POLARIS_ENGINE_CAMERA_H
#define POLARIS_ENGINE_CAMERA_H

#include "core.h"
#include "precision.h"
namespace PolarisEngine
{
	struct Rectangle
	{
		int x, y, w, h;	
	};
	
	struct Viewport
	{
		Viewport(int x, int y, int width, int height)
		{
	
		}
	
		Viewport(Rectangle bounds)
		{
			
		}

		float aspectRatio; //length / height;
		int height; //Height of the viewport
		float maxDepth; //Maximum depth of the clipping volume; Normally set to 1.0
		float minDepth; //minimum depth of the clipping volume; Normally set to 0.0
		Rectangle titleSafeArea; //Title safe area within screen
		int width; //WIdth of the viewport

		int X; //Upper left corner of viewport
		int Y; //upper left corner of viewport

		Vector3 Project(Vector3 source, Matrix4 projection, Matrix4 view, Matrix4 world);



	};
};
#endif