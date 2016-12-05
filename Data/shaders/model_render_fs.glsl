/**
 * This file is a part of a project from http://zompi.pl/light-shafts/
 * (c) 2014 Damian Nowakowski
 */

#version 150
 
/**
* This is structure that stores material parameters
*/
struct MaterialParameters
{
    vec4 emission;
    vec4 ambient;
    vec4 diffuse; 
    vec4 specular; 
    float shininess;
};

/**
* This is structure that stores light source parameters
*/
struct LightSourceParameters
{
    vec4 ambient;               
    vec4 diffuse;               
    vec4 specular;              
    vec3 attenuation;
};

/**
* This is uniform layout storing all needed Shading parameters.
* Using this layout application will update the uniform.
*/
layout( shared ) uniform Shading
{
    MaterialParameters material;
    LightSourceParameters lightSource;
};


in vec3 inoutNormal;
in vec3 inoutPosition;

uniform vec4 eyePosition;
uniform vec4 lightPosition;
uniform bool occlusion;

out vec4 outColor;


void main(void)
{
	if (occlusion == true)
	{
		// When there is occlusion only render only black occlusion
		outColor = vec4(0,0,0,1);
	}
	else
	{
		/// If not render using nice Phong-Blinn shading.


		// Normalize normals, they will be needed soon.
		vec3 normal = normalize( inoutNormal );

		// Vector of light direction falling on this vertex.
		vec3 lightVec = lightPosition.xyz - inoutPosition;

		// And the length of this vector.
		float distance = length( lightVec );

		// When we have it's length lets normalize it, it will be needed soon.
		lightVec = normalize( lightVec );

		

		// Get the cosinus of the angle between normal and light direction.
		float NdotL = max( dot( normal, lightVec ), 0.0 );

		// Check if vertex is even lighted
		if (NdotL <= 0.)
		{
			// The angle is bigger than 90 degrees so the vertex is not lighted.
			outColor = vec4( 0.0 );
		}
		else
		{
			// Calculate the half vector between the light vector and the view vector and the cosinus between
			// the normal and the half vector. Thanks to the final value there is no need for finding the more 
			// computationally heavy reflection vector.
			vec3 halfVec = normalize( lightPosition.xyz + normalize( eyePosition.xyz - inoutPosition ) );
			float NdotH = max( dot( normal, halfVec ), 0.0 );

			// Get the light attenuation.
			float att = 1.0 / ( lightSource.attenuation[0] +
								lightSource.attenuation[1] * distance +
								lightSource.attenuation[2] * distance * distance );

			// Final light calculations.
			outColor = att * (	lightSource.ambient +
								NdotL * material.diffuse * lightSource.diffuse +
								material.specular * lightSource.specular * pow( NdotH, material.shininess ) ); 
		}

		// At the end add material ambient and emission for more realistic look.
		outColor += (material.ambient + material.emission);
	}
}