/**
* LightShafts example.
*
* This is a light source class. It handles all parameters
* and shaders for Phong-Blinn point light.
*
* (c) 2014 Damian Nowakowski
*/

#include "Light.h"
#include "Shaders.h"
#include "Engine.h"
#include "Scene.h"
#include "Window.h"
#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

/**
 * Simple constructor with initialization
 */
Light::Light()
{
	// Remember the configuration reader so we can use it in future.
	INIReader * localINIReader = ENGINE->config;

	/// Remember all describing light variables from the ini configuration file
	position =		glm::vec3(	localINIReader->GetReal("Light", "Pos_X", 0.0), 
								localINIReader->GetReal("Light", "Pos_Y", 0.0),
								localINIReader->GetReal("Light", "Pos_Z", 0.0));

	ambient =		glm::vec4(	localINIReader->GetReal("Light", "Ambient_R", 1.0),
								localINIReader->GetReal("Light", "Ambient_G", 1.0),
								localINIReader->GetReal("Light", "Ambient_B", 1.0),
								1);

	diffuse[0] =	(GLfloat)	(localINIReader->GetReal("Light", "Diffuse_R", 0));
	diffuse[1] =	(GLfloat)	(localINIReader->GetReal("Light", "Diffuse_G", 0));
	diffuse[2] =	(GLfloat)	(localINIReader->GetReal("Light", "Diffuse_B", 0));
	diffuse[3] =				1.0f;

	specular =		glm::vec4(	localINIReader->GetReal("Light", "Specular_R", 1.0),
								localINIReader->GetReal("Light", "Specular_G", 1.0),
								localINIReader->GetReal("Light", "Specular_B", 1.0),
								1);

	attenuation =	glm::vec3(	localINIReader->GetReal("Light", "ConstantAttenuation", 0.0),
								localINIReader->GetReal("Light", "LinearAttenuation", 0.0),
								localINIReader->GetReal("Light", "QuadraticAttenuation", 0.0));

	moveSpeed =		(GLfloat)	(localINIReader->GetReal("Light", "Speed", 0.1));

	// The Y scale is multiplied by the camera ratio, because the geometry shader that will generate
	// the marker is creating a billboard, so it won't pass the ViewProjection matrix to all verticies.
	// Without the ratio in scale the marker won't be a perfect circle.
	scale =			glm::vec2(	localINIReader->GetReal("Light", "Marker_Size_X", 1.0), 
								ENGINE->scene->camera->GetRatio()*localINIReader->GetReal("Light", "Marker_Size_Y", 1.0));

	/// Create the shader that will be used to render the light marker
	Shaders::AttachShader(shader, GL_VERTEX_SHADER, "data/shaders/light_marker_vs.glsl");
	Shaders::AttachShader(shader, GL_GEOMETRY_SHADER, "data/shaders/light_marker_gs.glsl");
	Shaders::AttachShader(shader, GL_FRAGMENT_SHADER, "data/shaders/light_marker_fs.glsl");
	Shaders::LinkProgram(shader);

	// Remember the location of vertex position in shader program
	vertex_loc = glGetAttribLocation(shader, "inPosition");

	// Generate all necessary buffors for shader
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	/// Fill the buffer with the vertex data, that is a position of point light.
	/// Only one point is needed, bedause the whole marker will be generated in geometry shader.
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBufferData(GL_ARRAY_BUFFER, sizeof(position), glm::value_ptr(position), GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertex_loc);

	glBindVertexArray(0);

	// Zero the movement direction
	moveDir = glm::vec3(0);
}

/**
 * Draw the point light marker (to see where is the source)
 */
void Light::DrawTheMarker()
{
	// Get the view projection matrix from camera. It will be used for proper transformation of light position.
	glm::mat4 viewProjectionMatrix	= ENGINE->scene->camera->GetViewProjectionMatrix();
	
	/// Draw the light marker using given view projection matrix, scale and the diffuse color of light.
	/// We do not translate the position of light using the model matrix, because it has only position and it can
	/// be pass originally.
	glUseProgram(shader);

		glUniformMatrix4fv(glGetUniformLocation(shader, "viewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
		glUniform2fv(glGetUniformLocation(shader, "scale"), 1, glm::value_ptr(scale));
		glUniform4fv(glGetUniformLocation(shader, "color"), 1, diffuse);

		glBindVertexArray(VAO);
			glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);

	glUseProgram(0);
}

/**
 * Update the position and the color of the light
 * @param deltaTime - time of current tick
 */
void Light::Update(float deltaTime)
{
	// Set the new position using the movement direction and the shift
	position += (moveDir * moveSpeed * deltaTime);

	/// Update the vertex buffer data with new position
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(position), glm::value_ptr(position), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

/**
 * Handle the input controlling light source position.
 * @returns true if there was an input.
 */
bool Light::HandleInput()
{
	// Remember the local glfw Window so we won't have to get it all the time
	GLFWwindow * localWindow = ENGINE->window->glfwWindow;

	// Zero the moving state. This state will be used to determine if there was an input.
	bool isMoving = false;

	// Zero movement direction for now.
	moveDir = glm::vec3(0);

	/// Below there are key bindings. Every key is setting movement direction and set
	/// the movement state to true, so the light position will be updated.

	///Bindings:
	// Y - forward
	// H - backward
	// G - left
	// J - right
	// I - up
	// K - down

	if (glfwGetKey(localWindow, GLFW_KEY_Y) == GLFW_PRESS)
	{
		moveDir.z += -1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_H) == GLFW_PRESS)
	{
		moveDir.z += 1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_G) == GLFW_PRESS)
	{
		moveDir.x += -1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_J) == GLFW_PRESS)
	{
		moveDir.x += 1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_I) == GLFW_PRESS)
	{
		moveDir.y += 1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_K) == GLFW_PRESS)
	{
		moveDir.y += -1;
		isMoving = true;
	}

	// Return if camera is moving and has to be updated
	return isMoving;
}

/**
 * Simple destructor clearing all data.
 */
Light::~Light()
{
	Shaders::DeleteShaders(shader);
	glDeleteProgram(shader);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
