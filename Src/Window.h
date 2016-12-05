#pragma once

/**
* LightShafts example.
*
* This is a window class. It creates and handles the window with proper
* opengl context.
*
* This file is a part of a project from http://zompi.pl/light-shafts/
* (c) 2014 Damian Nowakowski
*/

#include "Engine.h"

class Window
{
public:
	/**
	* Simple constructor and destructor
	*/
	Window();
	~Window();

	GLFWwindow* glfwWindow;	///< Handler of the glfw window (needed for most glfw function)

	/**
	* Initialize the window.
	* It can't be used inside constructor because we need an information
	* about window creation successful. When there was a problem the application
	* has to be closed.
	* @return tru if window and opengl context were created successfuly.
	*/
	bool Init();
};
