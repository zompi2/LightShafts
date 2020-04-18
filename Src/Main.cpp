/**
 * LightShafts example.
 *
 * This is the main file of LightShafts example.
 *
 * This file is a part of a project from http://zompi.pl/light-shafts/
 * (c) 2014
 */

#include "Engine.h"

/**
 * Start the application.
 */
int main()
{
	// Init application engine so it can run
	ENGINE_INIT

	// As long as the engine is running update it
	while (ENGINE->IsRunning() == true)
	{
		ENGINE->Poll();
	}

	// When engine has been stopped clear the memory
	ENGINE_CLEAN

	// Exit application with no errors
	exit(EXIT_SUCCESS);
	return 0;
}
