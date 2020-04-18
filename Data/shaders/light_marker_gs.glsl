/**
 * Geometry shader used to draw a light marker.
 * (c) 2014 Damian Nowakowski
 */

#version 150

/// One point generates trangle_strip with 41 vertices (it will be a circle)
layout( points ) in;
layout( triangle_strip, max_vertices = 41 ) out;

uniform mat4 viewProjectionMatrix;
uniform vec2 scale;

in vec4 inoutPosition[1];

const float PI02 = 0.3141593; ///< (2*PI)/20  (full circle devided by 20 pieces)

void main()
{
	// Calculate the screen position of the marker using ViewProjection matrix
	vec4 center = viewProjectionMatrix*inoutPosition[0];		

	// Remember the initial position of center of the light marker
	vec4 vertex = center;
	
	// Do it for every 20 vertices +1 closing
	for(int i=0; i<=20; i++)
	{
		// Set the position of the next vertex
		vertex.x = center.x + scale.x * sin(PI02 * i);
		vertex.y = center.y + scale.y * cos(PI02 * i);
	
		// Emit this vertex
		gl_Position = vertex;
		EmitVertex();
	
		// Emit the center vertex so the triangle strip will be valid
		gl_Position = center;
		EmitVertex();			
	}
	
	// Emit the final primitive
	EndPrimitive();
}