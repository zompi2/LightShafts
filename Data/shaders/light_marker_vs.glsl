/**
 * This file is a part of a project from http://zompi.pl/light-shafts/
 * (c) 2014 Damian Nowakowski
 */

#version 150

in vec3 inPosition;
out vec4 inoutPosition;

void main()
{
	// Just pass the position to the geometry shader.
	inoutPosition = vec4(inPosition,1);
}