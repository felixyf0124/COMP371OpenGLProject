
#version 330 core
  
layout (location = 0) in vec2 position;

uniform vec3 COLOR;
uniform mat4 Project_Matrix;
out vec4 col;
void main()
{
	
	col = vec4(COLOR,1.0f);
	
    gl_Position = Project_Matrix  * vec4(position.x, position.y, 0.0f, 1.0f);
}