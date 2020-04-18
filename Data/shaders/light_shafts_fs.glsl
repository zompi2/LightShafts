/**
 * Fragment shader of a post process that draws scene with light shafts.
 * (c) 2014 Damian Nowakowski
 */

#version 150
 
/** Texture coords of occlusion image */
in vec2 inoutTexCoord;

/** Output color of the pixel */
out vec4 outColor;

/** Position of the light source on screen */
uniform vec2 lightScreenPos;

/** Array of textures: 0 - occlusion scene, 1 - normal scene */
uniform sampler2DArray tex;

/**
* This is uniform layout storing all needed light shafts parameters.
* Using this layout application will update the uniform.
*/
layout( shared ) uniform ShaftsParams
{
    int samples;
	float exposure;
	float decay;
	float density;
	float weight;
};

void main(void)
{
	// Set the basic color
	outColor = vec4(0);

	// Get current texture coordinates.
	vec2 textCoo = inoutTexCoord.xy;

	// Calculate the vector that is a one step on vector from lightsource to
	// the pixel of image.
	vec2 deltaTextCoord = textCoo - lightScreenPos;
 	deltaTextCoord *= 1.0 /  float(samples) * density;
 	
	// Set up illumination decay factor.
	float illuminationDecay = 1.0;

	// Evaluate the summation of shadows from occlusion texture
	for(int i=0; i < samples ; i++)
  	{
		// Step sample location along ray.
		textCoo -= deltaTextCoord;

		// Retrieve sample at new location.  
		vec4 colorSample  = texture(tex, vec3( clamp(textCoo,0,1), 0 ) );
			
		// Apply sample attenuation scale/decay factors.
		colorSample  *= illuminationDecay * weight;

		// Accumulate combined color.  
		outColor += colorSample;

		// Update exponential decay factor.
		illuminationDecay *= decay;
 	}

	// Output final color with a further scale control factor.
	outColor *= exposure;

	// Get the avarage of color from calculated light scattering and normal scene.	
	outColor += texture( tex, vec3( inoutTexCoord, 1 ) );
	outColor *= 0.5;
}