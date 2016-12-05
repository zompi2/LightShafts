/**
* LightShafts example.
*
* This is a model class. It stores and draws simple teapot.
*
* This file is a part of a project from http://zompi.pl/light-shafts/
* (c) 2014 Damian Nowakowski
*/

#include "Model.h"
#include "Shaders.h"
#include "Engine.h"
#include "Scene.h"
#include "Window.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Teapot.h"

/**
* Simple constructor with initialization.
*/
Model::Model()
{
	// Remember the configuration reader so we can use it in the future.
	INIReader * localINIReader = ENGINE->config;

	/// Remember all describing model position and material from configuration ini file
	position =	glm::vec3(	localINIReader->GetReal("Model", "Pos_X", 0.0),
							localINIReader->GetReal("Model", "Pos_Y", 0.0),
							localINIReader->GetReal("Model", "Pos_Z", 0.0));

	material.ambient =		glm::vec4(	localINIReader->GetReal("Material", "Ambient_R", 1.0),
										localINIReader->GetReal("Material", "Ambient_G", 1.0),
										localINIReader->GetReal("Material", "Ambient_B", 1.0),
										localINIReader->GetReal("Material", "Ambient_A", 1.0));
													 
	material.diffuse =		glm::vec4(	localINIReader->GetReal("Material", "Diffuse_R", 1.0),
										localINIReader->GetReal("Material", "Diffuse_G", 1.0),
										localINIReader->GetReal("Material", "Diffuse_B", 1.0),
										localINIReader->GetReal("Material", "Diffuse_A", 1.0));

	material.specular =		glm::vec4(	localINIReader->GetReal("Material", "Specular_R", 1.0),
										localINIReader->GetReal("Material", "Specular_G", 1.0),
										localINIReader->GetReal("Material", "Specular_B", 1.0),
										localINIReader->GetReal("Material", "Specular_A", 1.0));

	material.shininess =	(GLfloat)localINIReader->GetReal("Material", "Shininess", 1.0);

	/// Create a shader for rendering this model
	Shaders::AttachShader(shader, GL_VERTEX_SHADER, "data/shaders/model_render_vs.glsl");
	Shaders::AttachShader(shader, GL_FRAGMENT_SHADER, "data/shaders/model_render_fs.glsl");
	Shaders::LinkProgram(shader);

	/// Remember locations of vertex positions, normals and shading parameters structure from shader.
	/// The shading parameters are where material and light parameters are stored.
	vertex_loc = glGetAttribLocation(shader, "inPosition");
	normal_loc = glGetAttribLocation(shader, "inNormal");
	GLuint shadingParamsIndex = glGetUniformBlockIndex(shader, "Shading");

	/// Generate all necessary buffors for shader
	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &UBO);


	/// Fill the element array buffer with teapot indicies
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapotHighIndices), teapotHighIndices, GL_STATIC_DRAW);

	/// Fill the buffer with teapot vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(teapotHighPosition), teapotHighPosition, GL_STATIC_DRAW);
		glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(vertex_loc);

	/// Fill the buffer with teapot normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(teapotHighNormal), teapotHighNormal, GL_STATIC_DRAW);
		glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(normal_loc);

	glBindVertexArray(0);

	/// Declare the space in uniform buffer for shader where light and material parameters will be stored
	/// Remember all parameters offsets so the uniform buffer can be easely fill after that.
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	// Get the needed size of uniform buffer (the size of data structure in shader)
	GLint shadingParamsSize;
	glGetActiveUniformBlockiv(shader, shadingParamsIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &shadingParamsSize);
	glBufferData(GL_UNIFORM_BUFFER, shadingParamsSize, NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	glUniformBlockBinding(shader, shadingParamsIndex, 0);	

	// Prepare the array for parameters indices and helper array with parameters names
	GLuint uniformsIndex[MODEL_UNIFORM_SIZE];
	const GLchar *uniformsName[MODEL_UNIFORM_SIZE] =
	{
		"material.emission",
		"material.ambient",
		"material.diffuse",
		"material.specular",
		"material.shininess",
		"lightSource.ambient",
		"lightSource.diffuse",
		"lightSource.specular",
		"lightSource.attenuation"
	};

	// Get uniform buffers parameters offsets for future easily filling and update
	glGetUniformIndices(shader, MODEL_UNIFORM_SIZE, uniformsName, uniformsIndex);
	glGetActiveUniformsiv(shader, MODEL_UNIFORM_SIZE, uniformsIndex, GL_UNIFORM_OFFSET, uniformsOffset);

	// Get the light so it can be easily used in future
	Light * light = ENGINE->scene->light;

	/// Fill the uniform buffer with all necessary data (material and shading parameters)
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[0], 16, glm::value_ptr(material.emission));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[1], 16, glm::value_ptr(material.ambient));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[2], 16, glm::value_ptr(material.diffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[3], 16, glm::value_ptr(material.specular));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[4], 4, &(material.shininess));

	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[5], 16, glm::value_ptr(light->ambient));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[6], 16, light->diffuse);
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[7], 16, glm::value_ptr(light->specular));
	glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[8], 12, glm::value_ptr(light->attenuation));

	// Unbind the buffer object so program won't use them unnecessarily
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

/**
* Update the uniform buffer with light parameters
* @oaram light - currently used for rendering point light
*/
void Model::UpdateBuffer(Light * light)
{
	/// Bind the object buffer and update the light diffuse color parameter so it
	/// will have color like in tweak bar. After that unbind buffer object so program won't 
	/// use them unnecessarily.
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[6], 16, light->diffuse);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
}

/**
* Draw the model.
* @param camera		- currently used for rendering camera
* @oaram light		- currently used for rendering point light
* @param occlusion	- true if only occlusion must be drawn
*/
void Model::Draw(Camera * camera, Light * light, bool occlusion)
{
	/// Create few metrices and vectors that will be needed for shader.

	// Model transformation matrix - the matrix describing the model position
	glm::mat4 modelMatrix = glm::mat4() * glm::translate(glm::mat4(), position);

	//Model view projection matrix - needed for calculating screen position of verticies
	glm::mat4 modelViewProjectionMatrix = camera->GetViewProjectionMatrix() * modelMatrix;

	/// Draw the model using all calculated parameters, buffers and flag deciding if this render pass is occlusion only.
	/// Remember to use glDrawElements method, because we are using indicies in elements array.
	glUseProgram(shader);

		glUniformMatrix4fv(glGetUniformLocation(shader, "modelViewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
		glUniform1i(glGetUniformLocation(shader, "occlusion"), occlusion);

		// This matrix and vectors are needed only when normal scene (no occlusion) is drawing.
		if (occlusion == false)
		{
			// Inversed model matrix - needed for calculating object's
			// positions relative to the model
			glm::mat4 inverseModelMatrix = glm::inverse(modelMatrix);

			//Observator position in model coordinates - needed for lighting calculations
			glm::vec4 eyePosition = glm::vec4(camera->position - position, 1) * inverseModelMatrix;

			//Light position in model coordinates - needed for lighting calculations
			glm::vec4 objectLightPosition = glm::vec4(light->position - position, 1) * inverseModelMatrix;

			glUniform4fv(glGetUniformLocation(shader, "lightPosition"), 1, glm::value_ptr(objectLightPosition));
			glUniform4fv(glGetUniformLocation(shader, "eyePosition"), 1, glm::value_ptr(eyePosition));
		}

		glBindVertexArray(VAO);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
			glDrawElements(GL_TRIANGLES, teapotHighIndicesCount * 3, GL_UNSIGNED_INT, NULL);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
		glBindVertexArray(0);

	glUseProgram(0);
}

/**
* Simple destructor clearing all data.
*/
Model::~Model()
{
	Shaders::DeleteShaders(shader);
	glDeleteProgram(shader);
	glDeleteBuffers(3, VBO);
	glDeleteBuffers(1, &UBO);
	glDeleteVertexArrays(1, &VAO);
}
