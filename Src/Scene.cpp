/**
* LightShafts example.
*
* This is a scene class. It contains all elements displaying on the scene
* like model and light.
*
* (c) 2014 Damian Nowakowski
*/

#include "Scene.h"
#include "Window.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "Shaders.h"
#include "LightShafts.h"

/**
* Initialize the scene
* It can't be used in constructor because many objects created inside the scene
* needs the reference to itself.
*/
void Scene::Init()
{
	// Remember the configuration reader so we can use it in future.
	INIReader * localINIReader = ENGINE->config;

	/// Get the background color (the clear color) from the configuration ini file
	bgColor[0] = GLfloat(localINIReader->GetReal("Render", "ClearColor_R", 0));
	bgColor[1] = GLfloat(localINIReader->GetReal("Render", "ClearColor_G", 0));
	bgColor[2] = GLfloat(localINIReader->GetReal("Render", "ClearColor_B", 0));
	bgColor[3] = GLfloat(localINIReader->GetReal("Render", "ClearColor_A", 1));

	/// Create all objects that are on scene
	camera		= new Camera();
	light		= new Light();
	model		= new Model();
	lightShafts = new LightShafts();

}

/**
* Update the scene in this tick.
* @param deltaTime - the time of passed tick.
*/
void Scene::OnRun(double deltaTime)
{
	// When there was input in camera update it
	if (camera->HandleInput() == true)
	{
		camera->Update((float)deltaTime);
	}

	// When there was input in light update it position
	if (light->HandleInput() == true)
	{
		light->Update((float)deltaTime);
	}

	// Update the buffer object of model 
	// (because the color of the light can be changed)
	model->UpdateBuffer(light);

	// Update the buffer object of the lightshafts effect
	// (because parameters can be changed)
	lightShafts->UpdateUniformBuffer();
}

/**
* Draw whole scene.
*/
void Scene::OnDraw()
{
	// Draw the normal scene to the texture
	// (no need for rendering point light twice)
	lightShafts->StartDrawingNormal(this);
	model->Draw(camera, light, false);

	// Draw the occlusion to the texture
	lightShafts->StartDrawingOcclusion(this);
	light->DrawTheMarker();
	model->Draw(camera, light, true);

	// Compose these two textures and draw the final lightshafts scene
	lightShafts->DrawLightShafts(camera, light);
}

/**
* Simple destructor clearing all data.
*/
Scene::~Scene()
{
	delete camera;
	delete light;
	delete model;
	delete lightShafts;
}