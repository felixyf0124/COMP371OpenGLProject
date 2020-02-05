#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <stdio.h>
#include <glew.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <glm.hpp>
//#include "gtc/matrix_transform.hpp"
//#include "gtc/type_ptr.hpp"


using namespace std;

class Shader
{
public:
	unsigned int shaderID;
	
	Shader(const std::string vertex_shader_path, const std::string fragment_shader_path);
	~Shader();
};

#endif