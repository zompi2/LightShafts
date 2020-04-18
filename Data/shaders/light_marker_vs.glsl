/**
 * Vertex shader used to draw a light marker.
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