#pragma once

/**
* LightShafts example.
*
* This is a model class. It stores and draws simple teapot.
*
* (c) 2014 Damian Nowakowski
*/

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "Camera.h"
#include "Light.h"

// Define the uniform buffer elements of model shader
#define MODEL_UNIFORM_SIZE 9

class Model
{
public:
	/**
	* Simple constructor and destructor
	*/
	Model();
	~Model();

	glm::vec3 position;			///< Position of the model

	/**
	* Structure that holds model's material parameters
	*/
	struct MaterialParameters
	{
		glm::vec4 emission;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		GLfloat shininess;
	} material;					///< The material's parameters instance

	/**
	* Update the uniform buffer with light parameters
	* @param light - currently used for rendering point light
	*/
	void UpdateBuffer(Light * light);

	/**
	* Draw the model.
	* @param camera		- currently used for rendering camera
	* @param light		- currently used for rendering point light
	* @param occlusion	- true if only occlusion must be drawn
	*/
	void Draw(Camera * camera, Light * light, bool occlusion);

private:
	GLuint shader;		///< Handler of the shader that draws the model	

	GLuint VAO;			///< Vertex array object for shader that renders the model
	GLuint VBO[3];		///< Vertex byffer object for shader that renders the model
						///< (for indicies, verticies, normals)
	GLuint UBO;			///< Uniform buffer object for shader that render the model
						///< where material and light parameter are stored
	GLuint vertex_loc;	///< Vertex pointer needed for shader
	GLuint normal_loc;	///< Normals pointer needed for shader

	GLint uniformsOffset[MODEL_UNIFORM_SIZE];	///< Array that stores offsets of values in uniform buffer
};
