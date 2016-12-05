#pragma once

/**
* LightShafts example.
*
* This is an tweaking bar class that handles the AntTweakBar, initialize it
* and set up the input.
*
* This file is a part of a project from http://zompi.pl/light-shafts/
* (c) 2014 Damian Nowakowski
*/


#include <AntTweakBar.h>

// Predefine classes for visibility
class Engine;

class TweakBar
{
public:
	/**
	* Simple constructor with initialization
	* @param engine -	the pointer to the engine so tweak bar will have
	*					access to all objects
	*/
	TweakBar(Engine * engine);

	/**
	* Draw the tweak bar.
	*/
	void OnDraw();

	/**
	* Destructor when tweak bar is terminated.
	*/
	~TweakBar();
private:
	TwBar *bar;	///< Handler of Ant Tweak Bar instance
};

