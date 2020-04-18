/**
* LightShafts example.
*
* This is a light shafts renderer class. It handles all parameters
* and shaders for lightshafts and helper methods for rendering into
* proper render buffer.
*
* This file is a part of a project from http://zompi.pl/light-shafts/
* (c) 2014 Damian Nowakowski
*/

#include "LightShafts.h"
#include "Shaders.h"
#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "glm/gtc/type_ptr.hpp"

///< Vertex coordinates of final scene (quad filling whole screen)
GLfloat rect[12] =
{
	-1.0f, -1.0f,
	1.0f, -1.0f,
	1.0f, 1.0f,

	-1.0f, -1.0f,
	1.0f, 1.0f,
	-1.0f, 1.0f
};

///< Texture coordinates of final scene (quad filling whole screen)
GLfloat texcoords[12] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

/**
* Simple constructor with initialization
*/
LightShafts::LightShafts()
{
	// Remember the configuration reader so we can use it in the future.
	INIReader * localINIReader = ENGINE->config;

	/// Remember all describing light shafts effect variables from the ini configuration file
	backLightColor	= (GLfloat)localINIReader->GetReal("Shafts", "BackLightColor", 0);
	exposure		= (GLfloat)localINIReader->GetReal("Shafts", "Exposure", 0);
	decay			= (GLfloat)localINIReader->GetReal("Shafts", "Decay", 0);
	density			= (GLfloat)localINIReader->GetReal("Shafts", "Density", 0);
	weight			= (GLfloat)localINIReader->GetReal("Shafts", "Weight", 0);
	samples			= localINIReader->GetInteger("Shafts", "Samples", 0);

	// Remember the camera from scene so we can use it in the future
	Camera * localCamera = ENGINE->scene->camera;

	// Generate texture handlers for color and depth components
	glGenTextures(1, &renderTextureArrayColor);
	glGenTextures(1, &renderTextureArrayDepth);

	// Make a texture array for color component. This is where occlusion and scenes will be rendered to.
	glBindTexture(GL_TEXTURE_2D_ARRAY, renderTextureArrayColor);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, localCamera->renderWidth, localCamera->renderHeight, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Make a texture array for depth component. This is where occlusion and scenes depth buffer will be rendered to.
	glBindTexture(GL_TEXTURE_2D_ARRAY, renderTextureArrayDepth);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, localCamera->renderWidth, localCamera->renderHeight, 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);


	// Generate two frame buffers that will be used for rendering to textures
	glGenFramebuffers(2, frameBuffers);

	// Bind first frame buffer to the first position of rexture array of color and depth. This is where occlusion will be rendered.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[0]);
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTextureArrayColor, 0, 0);
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderTextureArrayDepth, 0, 0);

	// Bind first frame buffer to the second position of rexture array of color and depth. This is where the normal scene will be rendered.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[1]);
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTextureArrayColor, 0, 1);
	glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, renderTextureArrayDepth, 0, 1);

	// Bind the basic frame buffer for now in order to not make a mess.
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	/// Create a shader for light shafts effect
	Shaders::AttachShader(shader, GL_VERTEX_SHADER, "data/shaders/light_shafts_vs.glsl");
	Shaders::AttachShader(shader, GL_FRAGMENT_SHADER, "data/shaders/light_shafts_fs.glsl");
	Shaders::LinkProgram(shader);

	/// Remember locations of vertex positions, texture coordinations and light shafts parameters structure from shader.
	vertex_loc = glGetAttribLocation(shader, "inPosition");
	texcoord_loc = glGetAttribLocation(shader, "inTexCoord");
	GLuint shaftsParamsIndex = glGetUniformBlockIndex(shader, "ShaftsParams");

	/// Generate all necessary buffors for data
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &UBO);

	/// Fill the buffer with the vertex data, that are positions of verticies of quad filling the whole screen.
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
		glVertexAttribPointer(vertex_loc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(vertex_loc);

	/// Fill the buffer with the texture coordinates data that are proper for quad filling the whole screen.
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(texcoord_loc);

	glBindVertexArray(0);

	/// Declare the space in uniform buffer for shader where light shafts parameter are stored
	/// Remember all parameters offsets so the uniform buffer can be easely fill after that.
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);

	// Get the needed size of uniform buffer (the size of data structure in shader)
	GLint shaftsParamsSize;
	glGetActiveUniformBlockiv(shader, shaftsParamsIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &shaftsParamsSize);
	glBufferData(GL_UNIFORM_BUFFER, shaftsParamsSize, NULL, GL_DYNAMIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	glUniformBlockBinding(shader, shaftsParamsIndex, 0);

	// Prepare the array for parameters indices and helper array with parameters names
	GLuint uniformsIndex[SHAFTS_UNIFORM_SIZE];
	const GLchar *uniformsName[SHAFTS_UNIFORM_SIZE] =
	{
		"samples",
		"exposure",
		"decay",
		"density",
		"weight"
	};

	// Get uniform buffers parameters offsets for future easily filling and update
	glGetUniformIndices(shader, SHAFTS_UNIFORM_SIZE, uniformsName, uniformsIndex);
	glGetActiveUniformsiv(shader, SHAFTS_UNIFORM_SIZE, uniformsIndex, GL_UNIFORM_OFFSET, uniformsOffset);

	// Unbind the buffer object so program won't use them unnecessarily
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Fill the uniform buffer with first values.
	UpdateUniformBuffer();
}

/**
* Update the uniform buffer with new light shafts parameters.
*/
void LightShafts::UpdateUniformBuffer()
{
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[0], 4, &samples);
		glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[1], 4, &exposure);
		glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[2], 4, &decay);
		glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[3], 4, &density);
		glBufferSubData(GL_UNIFORM_BUFFER, uniformsOffset[4], 4, &weight);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
}

/*
* Run this before rendering the occlusion. The occlusion will be saved
* to texture and used to compose the light shafts effect.
* @param scene - the current scene
*/
void LightShafts::StartDrawingOcclusion(Scene * scene)
{
	// Bind and clear the buffer for rendering occlusion
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[0]);
	glViewport(0, 0, scene->camera->renderWidth, scene->camera->renderHeight);
	glClearColor(	backLightColor*scene->light->diffuse[0], 
					backLightColor*scene->light->diffuse[1], 
					backLightColor*scene->light->diffuse[2], 
					backLightColor*scene->light->diffuse[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*
* Run this before rendering the normal scene. This scene will be saved
* to texture and used to compose the light shafts effect.
* @param scene - the current scene
*/
void LightShafts::StartDrawingNormal(Scene * scene)
{
	// Bind and clear the buffer for rendering normal scene
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffers[1]);
	glViewport(0, 0, scene->camera->renderWidth, scene->camera->renderHeight);
	glClearColor(scene->bgColor[0], scene->bgColor[1], scene->bgColor[2], scene->bgColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*
* After getting occlusion texture and normal scene texture run this method.
* It will compose previous textures and render the final scene on the screen.
* @param camera - currently using camera
* @param light	- currently using point light
*/
void LightShafts::DrawLightShafts(Camera * camera, Light * light)
{
	// Bind and clear the buffer (default one) for rendering the final scene
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glViewport(0, 0, camera->renderWidth, camera->renderHeight);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/// Get the screen position of point light in (0,1) coordinates. It is needed for future
	/// light shafts calculations in shader (because the whole effect is a post process).
	glm::vec4 lightNDCPosition = camera->GetViewProjectionMatrix() * glm::vec4(light->position, 1);
	lightNDCPosition /= lightNDCPosition.w;
	glm::vec2 lightScreenPosition = glm::vec2(
		(lightNDCPosition.x + 1) * 0.5,
		(lightNDCPosition.y + 1) * 0.5
		);

	/// Draw the final scene using the texture array containing the occlusion and normal scene.
	/// Also the screen position of point light is needed. Use vertex buffers with positions and
	/// texture coordinates of quad that fills whole screen.
	glUseProgram(shader);

		glBindTexture(GL_TEXTURE_2D_ARRAY, renderTextureArrayColor);
		glUniform1i(glGetUniformLocation(shader, "tex"), 0);
		glUniform2fv(glGetUniformLocation(shader, "lightScreenPos"), 1, glm::value_ptr(lightScreenPosition));

		glBindVertexArray(VAO);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);
}

/**
* Simple destructor clearing all data.
*/
LightShafts::~LightShafts()
{
	Shaders::DeleteShaders(shader);
	glDeleteProgram(shader);
	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &UBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &renderTextureArrayColor);
	glDeleteTextures(1, &renderTextureArrayDepth);
}