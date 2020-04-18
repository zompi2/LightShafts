#pragma once

/**
 * LightShafts example.
 *
 * This is a camera class. It stores all camera data, update it state
 * and handle camera movement.
 *
 * This file is a part of a project from http://zompi.pl/light-shafts/
 * (c) 2014 Damian Nowakowski
 */

// Perspective and rotate functions in GLM taking degrees are deprecated.
// This flag force using radians and removes the warning message.
#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include "glm/glm.hpp"

class Camera
{
public:
	/**
	 * Simple constructor
	 */
	Camera();

	int renderWidth;	///< Width of render target and camera
	int renderHeight;	///< Height of render target and camera
	glm::vec3 position;	///< Position of a camera
	
	/**
	 * Get projection matrix of this camera.
	 */
	glm::mat4 GetProjectionMatrix() { return projectionMatrix; }

	/*
	 * Get view matrix of this camera.
	 */
	glm::mat4 GetViewMatrix() { return viewMatrix; }

	/**
	 * Get multiplied matrixes of view and projection of this camera.
	 */
	glm::mat4 GetViewProjectionMatrix() { return viewProjectionMatrix; }
	
	/**
	 * Get aspect ratio of this camera.
	 */
	GLfloat GetRatio() { return ratio; }

	/**
	 * Update the position, rotation and matrices of this camera.
	 * @param deltaTime - the portion of time thas passed from previous update.
	 */
	void Update(float deltaTime);

	/**
	 * Handle the input controlling this camera.
	 * @returns true if there was an input.
	 */
	bool HandleInput();

private:

	GLfloat FOV;					///< Field Of View
	GLfloat ratio;					///< Aspect ratio
	GLfloat fnear;					///< Near plane of camera frustum
	GLfloat ffar;					///< Far plane of camera frustum

	glm::vec3 look;					///< The point on which camera is looking
	glm::vec3 up;					///< The direction of camera's up
	glm::vec2 rotationAngle;		///< XY angle of camera rotation in radians

	glm::vec3 originalLook;			///< The original point on which camera is looking
	glm::vec3 originalUp;			///< The original direction of camera's up

	GLfloat moveSpeed;				///< The speed of camera movement
	GLfloat rotateSpeed;			///< The speed of camera rotation

	glm::vec3 moveDir;				///< Current direction of camera movement
	glm::vec2 rotateDir;			///< Current direction of camera rotation

	glm::mat4 projectionMatrix;		///< Projection matrix of camera
	glm::mat4 viewMatrix;			///< View matrix of camera
	glm::mat4 viewProjectionMatrix;	///< Multiplied view and projection matrix

	double oldMouseX;				///< Remembered the previous X position of mouse for checking camera's rotation directory
	double oldMouseY;				///< Remembered the previous Y position of mouse for checking camera's rotation directory
};