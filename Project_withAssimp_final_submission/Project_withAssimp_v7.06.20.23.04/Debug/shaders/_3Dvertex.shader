
#version 330 core
  
layout (location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
//layout(location = 3) in vec3 normal;
layout(location = 2) in vec2 texCoordinate;

//uniform int origin;
uniform vec3 COLOR;
uniform mat4 Model_Matrix;
uniform mat4 View_Matrix;
uniform mat4 Project_Matrix;

out vec3 frag_pos;
out vec3 _norm;
out vec3 col;
out vec2 _texCoordinate;

void main()
{
	//col = COLOR;
	
	gl_Position = Project_Matrix * View_Matrix * Model_Matrix * vec4(position, 1.0f);
	frag_pos = vec3(Model_Matrix * vec4(position, 1.0f));
	_norm = mat3(transpose(inverse( Model_Matrix))) * normal;
	//_norm = normalize(transpose(inverse(mat3(Model_Matrix))) * normal);
	//_norm = vec3(Model_Matrix * vec4(normal, 1.0f));
	_texCoordinate = texCoordinate;

}