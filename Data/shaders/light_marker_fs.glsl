/**
 * Fragment shader used to draw a light marker.
 * (c) 2014 Damian Nowakowski
 */

#version 150
 
out vec4 outColor;
uniform vec4 color;

void main(void)
{
	// Just use the color given in the uniform
	outColor = color;
}