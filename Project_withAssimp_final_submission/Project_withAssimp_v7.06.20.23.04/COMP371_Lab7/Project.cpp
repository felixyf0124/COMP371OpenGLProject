//Lab 7 - Skybox

#include "stdafx.h"

#include <glew.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <../COMP371_Lab7/objloader.hpp>
#include <../soil/SOIL.h>
#include <cmath>

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"

#include "Collision.h"


using namespace std;

// Window dimensions
const GLuint WINDOW_WIDTH = 1000, WINDOW_HEIGHT = 1000;
const GLfloat CAMERA_MOVEMENT_STEP = 2.0f, movingStepMax = 0.1f;
const float ANGLE_ROTATION_STEP = 2.0f;
GLfloat movingStep;
//gravity coeficient
const GLfloat g = -9.8f;


int width, height;
GLuint myColID = 0;


static double currentTime;
//(GLfloat)glfwGetTime();
static double xpos, ypos, lastX, lastY;
struct camMovRec
{
	double lastForward, lastBackward, lastLeftward, lastRightward;
	//double currentTime

};
camMovRec myCam;

static bool mouse_button_left = false, mouse_button_right = false, shift = false, COL_ID_PICKING = false, objMoving = false;
bool apt_draw_mode = false, _3DGen=true;

glm::vec3 color_picked;
GLuint ColPickCounter;
unsigned char centerCol[4];
GLfloat limitation = 0.05f;

vector<Model*> myScene;

//toggle
bool flashLight = false, livingR = true, bedR1 = false, bedR2 = false, kitchen = false, bathR = true;
//ploygonMode
bool pointMode = true, lineMode = true, faceMode = false, faceMode_MeshLine = false, menu = false;
//camera bool
bool moveForward = false, moveBackward = false, moveLeftward = false, moveRightward = false;
bool jump = false,  crouch = false;

//obj bool
bool objMoving_deOffset = false;

//camera offset
const GLfloat  jumpPeriod = 0.5f, jumpStartingSpeed = -g*jumpPeriod/2.0f;
GLfloat jumpOffset = 0.0f, crouchOffset = 0.0f, jumpTime=0.0f, jump_start_time;


static glm::vec3 currentCol = glm::vec3(0.5f, 0.5f, 0.5f);
//GLfloat floorThick = 0.3f;

const glm::vec3 posX = glm::vec3(1.0f, 0.0f, 0.0f), negX = glm::vec3(-1.0f, 0.0f, 0.0f);
const glm::vec3 posY = glm::vec3(0.0f, 1.0f, 0.0f), negY = glm::vec3(0.0f, -1.0f, 0.0f);
const glm::vec3 posZ = glm::vec3(0.0f, 0.0f, 1.0f), negZ = glm::vec3(0.0f, 0.0f, -1.0f);


const glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f);

GLfloat playerHeight = 1.60f;

glm::vec3 cameraPos = glm::vec3(-1.2f, playerHeight, -3.6f);
glm::vec3 cameraTar = glm::vec3(0.0f, playerHeight, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

Camera playerView(cameraPos, cameraTar, cameraUp);

glm::mat4 projection_matrix;

const glm::mat4 MODEL_MATRIX = glm::mat4(1.0f);
glm::mat4 model_matrix;

glm::mat4 modelMSet = MODEL_MATRIX;
glm::mat4 modelMSet_T = MODEL_MATRIX;
glm::mat4 modelMSet_R = MODEL_MATRIX;

float horiz_rotation_angle = 0.0f, vertic_rotation_angle = 0.0f; // of the model



//Gravity Acceleration
GLfloat GravityAcceleration(GLfloat time) {
	GLfloat distance = (1.0f / 2.0f)*g*(time*time);
	return distance;
}

//cam function
void jump_fallBack() {
	if (jump) {
		currentTime = (GLfloat)glfwGetTime();
		jumpTime = currentTime - jump_start_time;
		//cout << jumpTime << ", here is the time" << endl;
		if (jumpTime < jumpPeriod)
		{
			jumpOffset = jumpStartingSpeed*jumpTime + GravityAcceleration(jumpTime);
			playerView.CameraHeightUpdating(jumpOffset);
		}
		else
		{
			playerView.CameraHeightUpdating(0.0f);
			jump = false;
		}
		//cout << GravityAcceleration(jumpTime) << ", here is the offset" << endl;

	}
}


//callback functions
static void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	//cout << x << ", " << y << endl;
	xpos = x/100.0f;
	ypos = y/100.0f;
	if (xpos > 0 && xpos < width / 100.0f && ypos > 0 && ypos < height / 100.0f && mouse_button_left && apt_draw_mode)
	{
		//clear the elements except the first one


	}


	if (_3DGen /*&& ( mouse_button_right)*/ && !menu)
	{
		
		float tiltLeftNRight = -(xpos - lastX);
		float tiltUpNDown = ypos - lastY;
		glm::mat4 ORM = playerView.horizontalTilt(tiltLeftNRight*ANGLE_ROTATION_STEP);
		if (objMoving)
		{
			modelMSet_R *= ORM;
		}
		playerView.veritcalTilt(tiltUpNDown*ANGLE_ROTATION_STEP);
	}
	lastX = x / 100.0f;
	lastY = y / 100.0f;

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	std::cout << button << std::endl;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && _3DGen && !menu)
	{

			//COL_ID_PICKING = !COL_ID_PICKING;
		if (!objMoving)
		{
			COL_ID_PICKING = true;
			ColPickCounter = 2;
		}
		else
		{
			modelMSet_R = MODEL_MATRIX;
			modelMSet_T = MODEL_MATRIX;
			color_picked.x = 0;
			objMoving = false;
		}
	}

	

};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);


	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && jump==false)
	{
		jump = true;
		jump_start_time = (GLfloat)glfwGetTime();
	}

	if (_3DGen)
	{
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			moveForward = true;
			moveBackward = false;
			//moveLeftward = false;
			//moveRightward = false;

			myCam.lastForward = (GLfloat)glfwGetTime();
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			moveForward = false;

		}

		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			moveForward = false;
			moveBackward = true;
			//moveLeftward = false;
			//moveRightward = false;

			myCam.lastBackward = (GLfloat)glfwGetTime();

		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			moveBackward = false;
		}

		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			//moveForward = false;
			//moveForward = false;
			moveLeftward = true;
			moveRightward = false;

			myCam.lastLeftward = (GLfloat)glfwGetTime();
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			moveLeftward = false;
		}

		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			//moveForward = false;
			//moveForward = false;
			moveLeftward = false;
			moveRightward = true;

			myCam.lastRightward = (GLfloat)glfwGetTime();
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			moveRightward = false;
		}
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		menu = !menu;
		if(menu)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		flashLight = !flashLight;

	}

	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		faceMode = false;

		lineMode = true;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		pointMode = !pointMode;
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
		shift = true;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
		shift = false;

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		livingR = !livingR;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		bedR1 = !bedR1;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		bedR2 = !bedR2;
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		kitchen = !kitchen;
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		bathR = !bathR;

}
//loaders


//cubmap loader
GLuint loadCubemap(vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);

		SOIL_free_image_data(image); //free resources
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

//main function
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);


	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwSetKeyCallback(window, key_callback);
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	//glEnable(GL_CULL_FACE);


	//shaders loading
	Shader skyboxShaderProgram = Shader("shaders/skybox_vertex.shader", "shaders/skybox_fragment.shader");
	Shader _2DShader = Shader("shaders/_2Dvertex.shader", "shaders/_2Dfragment.shader");
	Shader _3DShader = Shader("shaders/_3Dvertex.shader", "shaders/_3Dfragment.shader");
	//Shader GUIShader = Shader("shaders/GUI_vertex.shader", "shaders/GUI_fragment.shader");
	Shader colorID = Shader("shaders/colorIDvertex.shader", "shaders/colorIDfragment.shader");
	//end of pre-setting


	cout << "It might take a few minutes to load. Please be patient. " << endl;

	cout << "Loading...... " << endl;

	//projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	Model myAptCeiling("model/ceiling/ceiling.obj", myColID,0 );
	myColID++;
	myScene.push_back(&myAptCeiling);
	cout << "Loading...... ceiling loaded " << endl;

	Model myAptFloor("model/floor/floor.obj", myColID, 0);
	myColID++;
	myScene.push_back(&myAptFloor);
	cout << "Loading...... floor loaded " << endl;

	Model myAptWall("model/wall/wall.obj", myColID, 0);
	myColID++;
	myScene.push_back(&myAptWall);
	cout << "Loading...... wall loaded " << endl;

	Model myAptWDframes("model/WDframes/WDframes.obj", myColID, 0);
	myColID++;
	myScene.push_back(&myAptWDframes);
	cout << "Loading...... window and door frames loaded" << endl;

	Model myToilet("model/toilet/toilet.obj", myColID, 1);
	myColID++;
	myScene.push_back(&myToilet);
	cout << "Loading...... toilet loaded" << endl;

	Model mySofa01("model/sofa/sofa1.obj", myColID, 2);
	myColID++;
	myScene.push_back(&mySofa01);
	cout << "Loading...... sofa1 loaded" << endl;

	Model mySofa02("model/sofa/sofa2.obj", myColID, 2);
	myColID++;
	myScene.push_back(&mySofa02);
	cout << "Loading...... sofa2 loaded" << endl;

	Model mySofa03("model/sofa/sofa3.obj", myColID, 2);
	myColID++;
	myScene.push_back(&mySofa03);
	cout << "Loading...... sofa3 loaded" << endl;

	Model mySofa04("model/sofa/sofa4.obj", myColID, 2);
	myColID++;
	myScene.push_back(&mySofa04);
	cout << "Loading...... sofa4 loaded" << endl;

	Model mySofa05("model/sofa/roundSofa.obj", myColID, 2);
	myColID++;
	myScene.push_back(&mySofa05);
	cout << "Loading...... roundSofa loaded" << endl;

	Model myTable1("model/tables/coffeeTable1.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myTable1);
	cout << "Loading...... coffeeTable1 loaded" << endl;

	Model myTable2("model/tables/coffeeTable2.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myTable2);
	cout << "Loading...... coffeeTable2 loaded" << endl;

	Model myTable3("model/tables/desk.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myTable3);
	cout << "Loading...... desk loaded" << endl;

	Model myTable4("model/tables/table.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myTable4);
	cout << "Loading...... table loaded" << endl;

	Model myChair1("model/chairs/chair1.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myChair1);
	cout << "Loading...... chair1 loaded" << endl;

	Model myChair2("model/chairs/chair2.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myChair2);
	cout << "Loading...... chair2 loaded" << endl;

	Model myChair3("model/chairs/chair3.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myChair3);
	cout << "Loading...... chair3 loaded" << endl;

	Model myBed1("model/bed/bed1.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myBed1);
	cout << "Loading...... bed1 loaded" << endl;

	Model myBed2("model/bed/bed2.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myBed2);
	cout << "Loading...... bed2 loaded" << endl;

	Model myBedTable1("model/bed/bedTable1.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myBedTable1);
	cout << "Loading...... bedTable1 loaded" << endl;

	Model myBedTable2("model/bed/bedTable2.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myBedTable2);
	cout << "Loading...... bedTable2 loaded" << endl;

	Model myBedTable3("model/bed/bedTable3.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myBedTable3);
	cout << "Loading...... bedTable3 loaded" << endl;

	Model myBedTable4("model/bed/bedTable4.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myBedTable4);
	cout << "Loading...... bedTable4 loaded" << endl;

	Model myFridge("model/kitch/fridge.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myFridge);
	cout << "Loading...... fridge loaded" << endl;

	Model myStove("model/kitch/stove.obj", myColID, 2);
	myColID++;
	myScene.push_back(&myStove);
	cout << "Loading...... stove loaded" << endl;

	cout << "Loading done. " << endl;

	
	vector<glm::vec3> wallBB;
	vector<glm::vec3> wallBBN;

	vector<glm::vec3> myVertex;
	vector<glm::vec3> VNtrash;
	vector<glm::vec2> UVtrash;

	loadOBJ("model/wallBB.obj", wallBB, wallBBN, UVtrash);



	//buffer setting

	



	std::vector<glm::vec3> skybox_vertices;
	std::vector<glm::vec3> skybox_normals; //unused
	std::vector<glm::vec2> skybox_UVs; //unused

	loadOBJ("model/cube.obj", skybox_vertices, skybox_normals, skybox_UVs);

	//prepare skybox VAO
	GLuint skyboxVAO, skyboxVerticesVBO;

	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);

	glGenBuffers(1, &skyboxVerticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVerticesVBO);
	glBufferData(GL_ARRAY_BUFFER, skybox_vertices.size() * sizeof(glm::vec3), &skybox_vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//prepare skybox cubemap
	vector<const GLchar*> faces;
	faces.push_back("right.jpg");
	faces.push_back("left.jpg");
	faces.push_back("top.jpg");
	faces.push_back("bottom.jpg");
	faces.push_back("back.jpg");
	faces.push_back("front.jpg");

	glActiveTexture(GL_TEXTURE1);
	GLuint cubemapTexture = loadCubemap(faces);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	//references to uniforms

	GLuint cID_color = glGetUniformLocation(colorID.shaderID, "COLOR");
	GLuint cID_modelLoc = glGetUniformLocation(colorID.shaderID, "Model_Matrix");
	GLuint cID_viewLoc = glGetUniformLocation(colorID.shaderID, "View_Matrix");
	GLuint cID_projectionLoc = glGetUniformLocation(colorID.shaderID, "Project_Matrix");


	GLuint color = glGetUniformLocation(_3DShader.shaderID, "COLOR");
	GLuint modelLoc = glGetUniformLocation(_3DShader.shaderID, "Model_Matrix");
	GLuint viewLoc = glGetUniformLocation(_3DShader.shaderID, "View_Matrix");
	GLuint projectionLoc = glGetUniformLocation(_3DShader.shaderID, "Project_Matrix");


	int count = 0;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//draw while loop
	while (!glfwWindowShouldClose(window))
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);



		if (_3DGen)
		{
			jump_fallBack();
			model_matrix = MODEL_MATRIX;


			//camera position movement + drag obj movement
			modelMSet_T = MODEL_MATRIX;
			//	modelMSet_R = MODEL_MATRIX;

			if (moveForward)
			{
				currentTime = (GLfloat)glfwGetTime();
				GLfloat timeGap = currentTime - myCam.lastForward;
				movingStep = CAMERA_MOVEMENT_STEP*timeGap;
				if (movingStep > movingStepMax)
					movingStep = movingStepMax;
				glm::vec3 camFPos = playerView.futurePosMovFroNBac(movingStep);
				if (!CollisionDetectCAMtoWALL(camFPos, playerView.camVol, wallBB)&&!collisionDetectCAMtoOBJ(camFPos, playerView.camVol, myScene))
				{
					glm::vec3 movDir = playerView.MoveFrontandBack(movingStep);
					if (objMoving)
					{
						//reset model_matrix *** very important
						model_matrix = MODEL_MATRIX;

						//glm::vec3 movDir = glm::vec3(playerView.Direction.x, 0.0f, playerView.Direction.z) * (CAMERA_MOVEMENT_STEP*timeGap);
						model_matrix = glm::translate(model_matrix, movDir);
						modelMSet_T *= model_matrix;
					}
				}
				myCam.lastForward = (GLfloat)glfwGetTime();
			}
			if (moveBackward)
			{
				currentTime = (GLfloat)glfwGetTime();
				GLfloat timeGap = currentTime - myCam.lastBackward;
				movingStep = CAMERA_MOVEMENT_STEP*timeGap;
				if (movingStep > movingStepMax)
					movingStep = movingStepMax;

				glm::vec3 camFPos = playerView.futurePosMovFroNBac(-movingStep);
				if (!CollisionDetectCAMtoWALL(camFPos, playerView.camVol, wallBB) && !collisionDetectCAMtoOBJ(camFPos, playerView.camVol, myScene))
				{
					glm::vec3 movDir = playerView.MoveFrontandBack(-movingStep);
					if (objMoving)
					{
						//reset model_matrix *** very important
						model_matrix = MODEL_MATRIX;

						//glm::vec3 movDir = glm::vec3(playerView.Direction.x, 0.0f, playerView.Direction.z) * (-CAMERA_MOVEMENT_STEP*timeGap);
						model_matrix = glm::translate(model_matrix, movDir);
						modelMSet_T *= model_matrix;
					}
				}
				myCam.lastBackward = (GLfloat)glfwGetTime();
			}

			if (moveLeftward)
			{
				currentTime = (GLfloat)glfwGetTime();
				GLfloat timeGap = currentTime - myCam.lastLeftward;
				movingStep = CAMERA_MOVEMENT_STEP*timeGap;
				if (movingStep > movingStepMax)
					movingStep = movingStepMax;
				glm::vec3 camFPos = playerView.futurePosMovLefNRig(-movingStep);
				if (!CollisionDetectCAMtoWALL(camFPos, playerView.camVol, wallBB) && !collisionDetectCAMtoOBJ(camFPos, playerView.camVol, myScene))
				{
					glm::vec3 movDir = playerView.MoveLeftandRight(-movingStep);
					if (objMoving)
					{
						//reset model_matrix *** very important
						model_matrix = MODEL_MATRIX;

						//glm::vec3 movDir = glm::vec3(playerView.Right.x, 0.0f, playerView.Right.z) * (CAMERA_MOVEMENT_STEP*timeGap);
						model_matrix = glm::translate(model_matrix, -movDir);
						modelMSet_T *= model_matrix;
					}
				}
				myCam.lastLeftward = (GLfloat)glfwGetTime();
			}

			if (moveRightward)
			{
				currentTime = (GLfloat)glfwGetTime();
				GLfloat timeGap = currentTime - myCam.lastRightward;
				movingStep = CAMERA_MOVEMENT_STEP*timeGap;
				if (movingStep > movingStepMax)
					movingStep = movingStepMax;
				glm::vec3 camFPos = playerView.futurePosMovLefNRig(movingStep);
				if (!CollisionDetectCAMtoWALL(camFPos, playerView.camVol, wallBB) && !collisionDetectCAMtoOBJ(camFPos, playerView.camVol, myScene))
				{
					glm::vec3 movDir = playerView.MoveLeftandRight(movingStep);
					if (objMoving)
					{
						//reset model_matrix *** very important
						model_matrix = MODEL_MATRIX;

						//glm::vec3 movDir = glm::vec3(playerView.Right.x, 0.0f, playerView.Right.z) * (-CAMERA_MOVEMENT_STEP*timeGap);
						model_matrix = glm::translate(model_matrix, -movDir);
						modelMSet_T *= model_matrix;
					}
				}
				myCam.lastRightward = (GLfloat)glfwGetTime();
			}




			glUniform3fv(glGetUniformLocation(_3DShader.shaderID, "view_pos"), 1, glm::value_ptr(playerView.Position /*+ playerView.HeightOffset*/));
			glUniform3fv(glGetUniformLocation(_3DShader.shaderID, "camDir"), 1, glm::value_ptr(playerView.Direction /*+ playerView.HeightOffset*/));

			if (livingR)
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "livingR"), 1);
			else
			{
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "livingR"), 0);
			}
			if (bedR1)
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "bedR1"), 1);
			else
			{
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "bedR1"), 0);
			}
			if (bedR2)
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "bedR2"), 1);
			else
			{
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "bedR2"), 0);
			}
			if (kitchen)
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "kitchen"), 1);
			else
			{
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "kitchen"), 0);
			}
			if (bathR)
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "bathR"), 1);
			else
			{
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "bathR"), 0);
			}


			if (flashLight) {
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "flashOn"), 1);

			}
			else
			{
				glUniform1i(glGetUniformLocation(_3DShader.shaderID, "flashOn"), 0);

			}

			//generates view matrix
			glm::mat4 view_matrix = playerView.ViewMatrixGen();
			projection_matrix = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


			//for _3DShader
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(MODEL_MATRIX));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

			//for colorID
			glUniformMatrix4fv(cID_modelLoc, 1, GL_FALSE, glm::value_ptr(MODEL_MATRIX));
			glUniformMatrix4fv(cID_viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
			glUniformMatrix4fv(cID_projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));


			//colorID draw
			if (COL_ID_PICKING)
			{
				glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


				glUseProgram(colorID.shaderID);



				model_matrix = MODEL_MATRIX;

				for (GLuint i = 0; i < myScene.size(); i++)
				{


					if (myScene.at(i)->MOVABLE != 2)
					{
						myScene.at(i)->DrawColID(colorID, white);

					}
					else
					{

						myScene.at(i)->DrawColID(colorID, myScene.at(i)->refCol);

					}

				}
				glBindVertexArray(0);

				//glfwSwapBuffers(window);
					//picking colorID
				if (ColPickCounter == 1)
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

					glReadPixels(width / 2, height / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, centerCol);

					color_picked.x = (int)centerCol[0];
					color_picked.y = (int)centerCol[1];
					color_picked.z = (int)centerCol[2];



					cout << "col picking: " << color_picked.x << ", " << color_picked.y << ", " << color_picked.z << endl;
				}
					for (GLuint i = 0; i < myScene.size(); i++)
					{
						if (myScene.at(i)->COLOR_ID == color_picked.x && myScene.at(i)->MOVABLE == 2 && color_picked.x != 255)
							objMoving = true;
					}
				
					//modelMSet_R = MODEL_MATRIX;
					ColPickCounter--;

					if(ColPickCounter<1)
					COL_ID_PICKING = false;
				//glfwSwapBuffers(window);

			}
			else
			{

				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				////glUseProgram(_3DShader.shaderID);




					//draw skybox
				glUseProgram(skyboxShaderProgram.shaderID);
				glDisable(GL_CULL_FACE);


				glm::mat4 skybox_view = glm::mat4(glm::mat3(view_matrix)); //remove the translation data
					//glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(skybox_view));

				glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram.shaderID, "view_matrix"), 1, GL_FALSE, glm::value_ptr(skybox_view));
				glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram.shaderID, "projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection_matrix));

				glUniform1i(glGetUniformLocation(skyboxShaderProgram.shaderID, "skyboxTexture"), 1); //use texture unit 1

				glDepthMask(GL_FALSE);

				glBindVertexArray(skyboxVAO);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				glDrawArrays(GL_TRIANGLES, 0, skybox_vertices.size());
				glBindVertexArray(0);

				glDepthMask(GL_TRUE);

				glUseProgram(_3DShader.shaderID);


				glEnable(GL_CULL_FACE);


				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(MODEL_MATRIX));
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
				glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

				for (GLuint i = 0; i < myScene.size(); i++)
				{

					if (i == color_picked.x && objMoving)
					{
						model_matrix = MODEL_MATRIX;
						glm::vec3 camPos_offset = glm::vec3(playerView.getPosition().x, 0.0f, playerView.getPosition().z);
						//offset camera position as obj's rotation origin 

						model_matrix *= glm::translate(MODEL_MATRIX, camPos_offset);

						model_matrix *= modelMSet_R;

						model_matrix *= glm::translate(MODEL_MATRIX, -camPos_offset);
						model_matrix *= modelMSet_T;
						//model_matrix = modelMSet_R[i] * model_matrix;

						modelMSet = model_matrix;

						myScene[i]->BBupdate(modelMSet, false);
						if (CollisionDetectOBJtoWALL(*myScene[i], limitation, wallBB) || collisionDetectOBJtoOBJ(i, limitation, myScene))
						{
							objMoving = false;
							myScene[i]->BBupdate(modelMSet, true);

						}
						else
						{
							myScene[i]->objMM = modelMSet* myScene[i]->objMM;
						}
					}
					
					myScene.at(i)->Draw(_3DShader);
				}
				modelMSet_R = MODEL_MATRIX;

			}
	
			
			
		}

		

		// Swap the screen buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}
