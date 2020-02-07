
#version 330 core
  
layout (location = 0) in vec3 position;


//uniform int origin;
uniform vec3 COLOR;
uniform mat4 Model_Matrix;
uniform mat4 View_Matrix;
uniform mat4 Project_Matrix;

out vec3 col;

void main()
{
	col = COLOR;
	
	gl_Position = Project_Matrix * View_Matrix * Model_Matrix * vec4(position, 1.0f);


}