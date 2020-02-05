#include "stdafx.h"
#include "Camera.h"
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



Camera::Camera()
{
};


Camera::Camera(glm::vec3 camPos, glm::vec3 camTar, glm::vec3 camUp)
{

	Position = camPos;
	Target = camTar;
	Up = camUp;
	Direction = glm::normalize(Target - Position);
	Right = glm::normalize(glm::cross(Up, Direction));
};
//tilt horizontally along y-axis
glm::mat4 Camera::horizontalTilt(float AngInDeg) {

	glm::mat4 camRot = glm::mat4(1.0f);
	camRot = glm::rotate(camRot, glm::radians(AngInDeg), glm::vec3(0.0f, 1.0f, 0.0f));

	Direction = glm::normalize(glm::vec3(camRot * glm::vec4(Direction,1.0f)));
	Right = glm::normalize(glm::cross(Up, Direction));

	Target = Position + Direction;
	return camRot;
};
//tilt vertically along cameraRight
glm::mat4 Camera::veritcalTilt(float AngInDeg) {
	glm::mat4 camRot = glm::mat4(1.0f);
	camRot = glm::rotate(camRot, glm::radians(AngInDeg), Right);
	
	Direction = glm::normalize(glm::vec3(camRot * glm::vec4(Direction, 1.0f)));
	//Up = glm::normalize(glm::vec3(camRot * glm::vec4(Up, 1.0f)));

	Target = Position + Direction;
	return camRot;
};

glm::vec3 Camera::MoveFrontandBack(float movStep) {
	glm::vec3 moveDir = glm::vec3(Direction.x, 0.0f, Direction.z);
	glm::vec3 move = moveDir * movStep;
	Position = Position + move;
	Target = Target + move;
	return move;
};

glm::vec3 Camera::MoveLeftandRight(float movStep) {
	glm::vec3 move =  Right * movStep;
	Position = Position - move;
	Target = Target - move;
	return move;
};

// return furture position front & back movement
glm::vec3 Camera::futurePosMovFroNBac(float movStep) {
	glm::vec3 moveDir = glm::vec3(Direction.x, 0.0f, Direction.z);
	glm::vec3 FPos = Position + moveDir * movStep;
	return FPos;
}
// return furture position left & right movement
glm::vec3 Camera::futurePosMovLefNRig(float movStep) {
	glm::vec3 FPos = Position - Right * movStep;
	return FPos;
}



void Camera::CameraHeightUpdating(GLfloat offset) {
	HeightOffset = offset;
}

glm::vec3 Camera::getPosition() {
	return Position;
}




//generate viewMatrix
glm::mat4 Camera::ViewMatrixGen() {
	glm::vec3 height_offset = glm::vec3(0.0f, HeightOffset, 0.0f);

	glm::mat4 view = glm::lookAt(Position + height_offset, Target + height_offset, Up);
	return view;
};

Camera::~Camera()
{
};
