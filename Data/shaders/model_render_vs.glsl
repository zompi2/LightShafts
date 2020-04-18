/**
 * Vertex shader used to draw a 3D model.
 * (c) 2014 Damian Nowakowski
 */

#version 150

uniform mat4 modelViewProjectionMatrix;

in vec3 inPosition;
in vec3 inNormal;

out vec3 inoutPosition;
out vec3 inoutNormal;

void main()
{
	// Calculate the verticies screen position
	gl_Position = modelViewProjectionMatrix * vec4(inPosition,1);

	// Pass the original position and normals to the fragment shader (needed for lighting calculations)
	inoutPosition = inPosition;
	inoutNormal = inNormal;
}