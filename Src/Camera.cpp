/**
 * LightShafts example.
 *
 * This is a camera class. It stores all camera data, update it state
 * and handle camera movement.
 *
 * This file is a part of a project from http://zompi.pl/light-shafts/
 * (c) 2014 Damian Nowakowski
 */

#include "Camera.h"
#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

// Shortcut for transforming vec3 using mat4
#define TRANSFORM(v,m) (glm::vec3)(glm::vec4(v, 1.0f) * m)


/*
 * Simple constructor with initialization
 */
Camera::Camera()
{
	// Save local ini reader so we won't have to get it every time
	INIReader * localINIReader = ENGINE->config;

	// Remember the render(camera) width and haight. It will be used many times after that.
	renderWidth		= (int)localINIReader->GetInteger("Camera", "Width", 640);
	renderHeight	= (int)localINIReader->GetInteger("Camera", "Height", 480);

	// Get from ini file or set frustum parameters and remember them
	FOV		= (GLfloat)glm::radians((GLfloat)localINIReader->GetReal("Camera", "FOV", 90.0));
	ratio	= (GLfloat)renderWidth / (GLfloat)renderHeight;
	fnear	= (GLfloat)localINIReader->GetReal("Camera", "Near", 1.0);
	ffar	= (GLfloat)localINIReader->GetReal("Camera", "Far", 64.0);

	// Remember the initial position and rotation angle from ini file
	position		= glm::vec3((GLfloat)localINIReader->GetReal("Camera", "Pos_X", 0.0), (GLfloat)localINIReader->GetReal("Camera", "Pos_Y", 0.0), (GLfloat)localINIReader->GetReal("Camera", "Pos_Z", 2.0));
	rotationAngle	= glm::vec2((GLfloat)localINIReader->GetReal("Camera", "Rot_X", 0.0), (GLfloat)localINIReader->GetReal("Camera", "Rot_Y", 0.0));

	// Remember the movement and rotation speed from ini file
	moveSpeed		= (GLfloat)localINIReader->GetReal("Camera", "Speed", 10.0);
	rotateSpeed		= (GLfloat)localINIReader->GetReal("Camera", "Rot_Speed", 10.0);

	// Set movement and rotation directories to zero. Camera is not moving at the beginning.
	moveDir			= glm::vec3(0);
	rotateDir		= glm::vec2(0);

	// The original look is always straight into the scene
	originalLook = glm::vec3(0.0f, 0.0f, -1.0f);

	// The original up of the camera is always straight up
	originalUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// Set the projection matrix, we will use it many times after that.
	projectionMatrix = glm::mat4() * glm::perspective(FOV, ratio, fnear, ffar); 

	// Run update method once so few properties will be set at the beginning
	Update(0);
}

/**
 * Update the position, rotation and matrices of this camera.
 * @param deltaTime - the portion of time thas passed from previous update.
 */
void Camera::Update(float deltaTime)
{
	// Add the rotation angle based on the direction, speed and time
	rotationAngle += rotateDir*rotateSpeed*deltaTime;

	// Clamp Y rotation angle so camera will not make a flip
	rotationAngle.y = glm::clamp(rotationAngle.y, -glm::half_pi<GLfloat>(), glm::half_pi<GLfloat>());
	
	// Create camera rotation matrix
	glm::mat4 cameraRotation = 
		glm::rotate(glm::mat4(1.0f), this->rotationAngle.y,  glm::vec3(1.0f, 0.0f, 0.0f))*
		glm::rotate(glm::mat4(1.0f), this->rotationAngle.x,  glm::vec3(0.0f, 1.0f, 0.0f));

	// Move camera in correct direction rotated by the angle (rotation matrix)
	position	+= TRANSFORM(moveDir*moveSpeed*deltaTime, cameraRotation);

	// Set the point where camera is looking based on camera position and rotated original look.
	look		= position + TRANSFORM(originalLook, cameraRotation);

	// Set the camera's up, simply rotate the original up.
	up			= TRANSFORM(originalUp, cameraRotation);

	// Set the new camera view matrix based on new position and direction
	viewMatrix = glm::mat4() * glm::lookAt(position, look, up);

	// Multiply projection and view matrix to get complete ViewProjectionMatrix.
	// This matrix is commonly used in transforming verticies based on camera position
	// and perspective.
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

/**
 * Handle the input controlling this camera
 * @returns true if there was an input.
 */
bool Camera::HandleInput()
{
	// Remember the local glfw Window so we won't have to get it all the time
	GLFWwindow * localWindow = ENGINE->window->glfwWindow;

	// Zero the moving state. This state will be used to determine if there was an input.
	bool isMoving = false;

	// Zero movement and rotation directions for now.
	moveDir		= glm::vec3(0);
	rotateDir	= glm::vec2(0);

	// When the right mouse button was pressed calculate the difference between
	// current and previous position. This difference is the camera rotation direction.
	// Remember to set moving state to true, because camera has been moved.
	if (glfwGetMouseButton(localWindow, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		double mouseX, mouseY;
		glfwGetCursorPos(localWindow, &mouseX, &mouseY);

		rotateDir.x = (float)(mouseX - oldMouseX);
		rotateDir.y = (float)(mouseY - oldMouseY);

		oldMouseX = mouseX;
		oldMouseY = mouseY;

		isMoving = true;	
	}
	else
	{
		// Always remember the mouse position so when the mouse will be clicked
		// there won't be jump in camera rotation.
		glfwGetCursorPos(localWindow, &oldMouseX, &oldMouseY);
	}

	/// Below there are key bindings. Every key is setting movement direction and set
	/// the movement state to true, so the camera will be updated.

	///Bindings:
	// W - forward
	// S - backward
	// A - left
	// D - right
	
	if (glfwGetKey(localWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir.z += -1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir.z += 1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir.x += -1;
		isMoving = true;
	}

	if (glfwGetKey(localWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir.x += 1;
		isMoving = true;
	}

	// Return if camera is moving and has to be updated
	return isMoving;
}