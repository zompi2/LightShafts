#pragma once

/**
* LightShafts example.
*
* This is a scene class. It contains all elements displaying on the scene
* like model and light.
*
* This file is a part of a project from http://zompi.pl/light-shafts/
* (c) 2014 Damian Nowakowski
*/

#include "Engine.h"

// Predefine classes for visibility
class Camera;
class Light;
class Model;
class LightShafts;

class Scene
{
public:
	/**
	* Simple destructor
	*/
	~Scene();

	float			bgColor[4];		///< Background color used in clearing scene (RGBA).

	Camera*			camera;			///< Handler of the camera in the scene.
	Light*			light;			///< Handler of the point light in the scene.
	Model*			model;			///< Handler of the model in the scene.
	LightShafts*	lightShafts;	///< Handler of the lightshafts effect used in the scene.

	/**
	* Initialize the scene
	* It can't be used in constructor because many objects created inside the scene
	* needs the reference to itself.
	*/
	void Init();

	/**
	* Update the scene in this tick.
	* @param deltaTime - the time of passed tick.
	*/
	void OnRun(double deltaTime);

	/**
	* Draw whole scene.
	*/
	void OnDraw();

	
};
