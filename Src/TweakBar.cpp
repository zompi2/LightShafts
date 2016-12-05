/**
* LightShafts example.
*
* This is an tweaking bar class that handles the AntTweakBar, initialize it
* and set up the input.
*
* This file is a part of a project from http://zompi.pl/light-shafts/
* (c) 2014 Damian Nowakowski
*/


#include "TweakBar.h"
#include "Engine.h"
#include "Window.h"
#include "Scene.h"
#include "Camera.h"
#include "LightShafts.h"
#include "Light.h"
#include "Model.h"
#include <GLFW/glfw3.h>

/**
* Function that redirects the mouse button input from glfw to tweak bar
*/
void MouseButtonHandler(GLFWwindow * glfwWindow, int glfwButton, int glfwAction, int glfwMods)
{
	TwEventMouseButtonGLFW(glfwButton, glfwAction);
}

/**
* Function that redirects the mouse move from glfw to tweak bar
*/
void MousePositionHandler(GLFWwindow * glfwWindow, double mouseX, double mouseY)
{
	TwEventMousePosGLFW((int)mouseX, (int)mouseY);
}

/**
* Simple constructor with initialization
* @param engine -	the pointer to the engine so tweak bar will have
*					access to all objects
*/
TweakBar::TweakBar(Engine * engine)
{
	// First Ant Tweak Bar has to be initialized
	TwInit(TW_OPENGL, NULL);

	// Tell the tweak bar the size of window so it will know how big tweak bar has to be.
	TwWindowSize(engine->scene->camera->renderWidth, engine->scene->camera->renderHeight);
	TwDefine(" GLOBAL help='To know how to make such lightshafts visit: www.zompi.pl' ");

	/// Redirect the mouse position and mouse button input
	glfwSetMouseButtonCallback(engine->window->glfwWindow, MouseButtonHandler);
	glfwSetCursorPosCallback(engine->window->glfwWindow, MousePositionHandler);

	// Create new tweak bar instance and remember it. It is needed for adding new fields to the tweak bar.
	bar = TwNewBar("TweakBar");

	/// The light shafts parameters group
	// Samples
	TwAddVarRW(bar, "Samples", TW_TYPE_INT32, &engine->scene->lightShafts->samples,
		" label='Samples' min=1 max=1000 step=1 group=LightShafParams");

	// Exposure
	TwAddVarRW(bar, "Exposure", TW_TYPE_FLOAT, &engine->scene->lightShafts->exposure,
		" label='Exposure' min=0.0001 max=1 step=0.0001 group=LightShafParams");

	// Decay
	TwAddVarRW(bar, "Decay", TW_TYPE_FLOAT, &engine->scene->lightShafts->decay,
		" label='Decay' min=0.0001 max=2 step=0.001 group=LightShafParams");

	// Density
	TwAddVarRW(bar, "Density", TW_TYPE_FLOAT, &engine->scene->lightShafts->density,
		" label='Density' min=0.0001 max=2 step=0.001 group=LightShafParams");

	// Weight
	TwAddVarRW(bar, "Weight", TW_TYPE_FLOAT, &engine->scene->lightShafts->weight,
		" label='Weight' min=0.0001 max=100 step=0.1 group=LightShafParams");

	// Backlight Color
	TwAddVarRW(bar, "BackLightColor", TW_TYPE_FLOAT, &engine->scene->lightShafts->backLightColor,
		" label='BackLightColor' min=0 max=1 step=0.01 group=LightShafParams");

	/// The background color
	TwAddVarRW(bar, "BGColor", TW_TYPE_COLOR4F, &engine->scene->bgColor,
		" label='BGColor'");

	/// The light color
	TwAddVarRW(bar, "LightColor", TW_TYPE_COLOR4F, &engine->scene->light->diffuse,
		" label='LightColor'");

	/// Position of the model
	TwAddVarRW(bar, "ModelX", TW_TYPE_FLOAT, &engine->scene->model->position.x, " label='ModelX' step=0.1 group=Model");
	TwAddVarRW(bar, "ModelY", TW_TYPE_FLOAT, &engine->scene->model->position.y, " label='ModelY' step=0.1 group=Model");
	TwAddVarRW(bar, "ModelZ", TW_TYPE_FLOAT, &engine->scene->model->position.z, " label='ModelZ' step=0.1 group=Model");
}

/**
* Draw the tweak bar.
*/
void TweakBar::OnDraw()
{
	TwDraw();
}

/**
* Destructor when tweak bar is terminated.
*/
TweakBar::~TweakBar()
{
	TwTerminate();
}
