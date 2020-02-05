
#version 330 core
  
layout (location = 0) in vec2 position;
layout(location = 1) in vec3 color;
//uniform mat4 Model_Matrix;
//uniform mat4 View_Matrix;
uniform mat4 Project_Matrix;
out vec4 col;
void main()
{
	/*if (COLOR == vec4(0.0, 0.0, 0.0, 1.0)) {
		col = vec4(color, 1);
	}
	else {*/
		col = vec4(color,1.0f);
	/*}*/
    gl_Position = Project_Matrix  * vec4(position.x, position.y, 0.0f, 1.0f);
}