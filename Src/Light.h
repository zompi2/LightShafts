#pragma once

/**
 * LightShafts example.
 *
 * This is a light source class. It handles all parameters
 * and shaders for Phong-Blinn point light.
 *
 * This file is a part of a project from http://zompi.pl/light-shafts/
 * (c) 2014 Damian Nowakowski
 */

#include <GL/glew.h>
#include "glm/glm.hpp"

class Light
{
public:

	/**
	 * Simple constructor and destructor
	 */
	Light();
	~Light();

	glm::vec3 position;		///< Position of the point light

	GLfloat diffuse[4];		///< Diffuse color RGBA of the light(in an array because of TweakBar compatibility)
	glm::vec4 ambient;		///< Ambient color of the light
	glm::vec4 specular;		///< Specular color of the light
	
	glm::vec3 attenuation;	///< Light attenuation {constant, linear, quadriatic}

	/**
	 * Draw the point light marker (to see where is the source)
	 */
	void DrawTheMarker();

	/**
	 * Update the position and the color of the light
	 * @param deltaTime - time of current tick
	 */
	void Update(float deltaTime);

	/**
	 * Handle the input controlling light source position.
	 * @returns true if there was an input.
	 */
	bool HandleInput();

private:

	GLuint shader;		///< Handler of the shader that draws light marker

	GLuint VAO;			///< Vertex array object needed for shader
	GLuint VBO;			///< Vertex buffer object needed for shader
	GLuint vertex_loc;	///< Vertex pointer needed for shader

	glm::vec2 scale;	///< Scale needed for proper marker rendering
	glm::vec3 moveDir;	///< Direction of light source moving
	GLfloat moveSpeed;	///< Speed of light source moving
};

