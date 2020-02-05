#version 330 core

in vec3 skyboxCoord;

out vec4 color;

uniform samplerCube skyboxTexture;

void main()
{
	color = texture(skyboxTexture, skyboxCoord);
} 