#ifndef CAMERA_H
#define CAMERA_H
#include <vector>
#include <stdio.h>
#include <cstring>
#include <glew.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <glm.hpp>
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
using namespace std;

class Camera
{

	
public:
	glm::vec3 Position;
	glm::vec3 Target;
	glm::vec3 Up;
	glm::vec3 Direction;
	glm::vec3 Right;
	GLfloat camVol = 0.2f;
	GLfloat HeightOffset = 0.0f;
	Camera();

	Camera::Camera(glm::vec3 camPos, glm::vec3 camTar, glm::vec3 camUp);


	glm::mat4 Camera::horizontalTilt(float AngInDeg);

	glm::mat4 Camera::veritcalTilt(float AngInDeg);

	glm::vec3 Camera::MoveFrontandBack(float movStep);

	glm::vec3 Camera::MoveLeftandRight(float movStep);

	glm::vec3 Camera::futurePosMovFroNBac(float movStep);

	glm::vec3 Camera::futurePosMovLefNRig(float movStep);

	void Camera::CameraHeightUpdating(GLfloat offset);

	glm::vec3 getPosition();

	glm::mat4 Camera::ViewMatrixGen();

	~Camera();


};
#endif

