#version 330 core

struct mMaterial{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirLight {
	vec3 direction;
};

struct PointLight {
	vec3 position;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;


};


uniform vec3 view_pos;
uniform vec3 camDir;
uniform int flashOn;
uniform int livingR;
uniform int bedR1;
uniform int bedR2;
uniform int bathR;
uniform int kitchen;


uniform sampler2D _texture;
uniform sampler2D texture_diffuse0;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_ambient0;

uniform sampler2D texture_ambient1;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;


uniform vec2 tilingScale;

in vec3 frag_pos;
in vec3 _norm;
in vec3 col;
in vec2 _texCoordinate;

out vec4 color;


float alpha = 8.0f;
float ambient_coef = 0.01f;
float diffuse_coef = 0.8f;
float specular_coef = 0.1f;

float attenuation_const = 1.0f;
float attenuation_linear = 0.09f;
float attenuation_quad = 0.032f;

vec3 roomLight = vec3(1.0f, 1.0f,1.0f);

vec3 flashLight = vec3(0.90f, 0.90f, 0.90f);

vec3 roadLightSource = vec3(0.6f, 0.6f, 0.6f);

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos, vec3 lightSource, vec3 ambientCoef, vec3 diffuseCoef, vec3 specularCoef);

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos, vec3 lightSource, vec3 ambientCoef, vec3 diffuseCoef, vec3 specularCoef);

void main()
{
	


	
	vec3 objectColA0 = vec3(texture(texture_ambient0, _texCoordinate));
	vec3 objectColA1 = vec3(texture(texture_ambient1, _texCoordinate));

	vec3 objectColD0 = vec3(texture(texture_diffuse0, _texCoordinate));
	vec3 objectColD1 = vec3(texture(texture_diffuse1, _texCoordinate));

	vec3 objectColS0 = vec3(texture(texture_specular0, _texCoordinate));
	vec3 objectColS1 = vec3(texture(texture_specular1, _texCoordinate));

	vec3 Normal = normalize(_norm);

	SpotLight myLight;
	PointLight LivingRoom;
	PointLight Bedroom1;
	PointLight Bedroom2;
	PointLight Kitchen;
	PointLight Bathroom;

	LivingRoom.position = vec3(0.0f, 2.2f, 0.0f);
	Bedroom1.position = vec3(-4.6f, 2.2f, 2.0f);
	Bedroom2.position = vec3(-4.6f, 2.2f, -2.2f);
	Kitchen.position = vec3(4.6f, 2.2f, 2.0f);
	Bathroom.position = vec3(5.75f, 2.2f, -2.2f);


	myLight.position = view_pos;
	myLight.direction = camDir;
	myLight.cutOff = cos(radians(20.0f));
	myLight.outerCutOff = cos(radians(30.0f));

	vec3 resultant = vec3(0.0f, 0.0f, 0.0f);

	

	if (flashOn == 1)
		resultant += CalcSpotLight(myLight, Normal, frag_pos, view_pos, flashLight, objectColA0, objectColD0, objectColS0);

	if(livingR == 1)
		resultant += CalcPointLight(LivingRoom, Normal, frag_pos, view_pos, roomLight, objectColA0+ objectColA1, objectColD0+ objectColD1, objectColS0+ objectColS1);

	if (bedR1 == 1)
		resultant += CalcPointLight(Bedroom1, Normal, frag_pos, view_pos, roomLight, objectColA0 + objectColA1, objectColD0 + objectColD1, objectColS0 + objectColS1);
	if (bedR2 == 1)
		resultant += CalcPointLight(Bedroom2, Normal, frag_pos, view_pos, roomLight, objectColA0 + objectColA1, objectColD0 + objectColD1, objectColS0 + objectColS1);
	if (kitchen == 1)
		resultant += CalcPointLight(Kitchen, Normal, frag_pos, view_pos, roomLight, objectColA0 + objectColA1, objectColD0 + objectColD1, objectColS0 + objectColS1);
	if (bathR == 1)
		resultant += CalcPointLight(Bathroom, Normal, frag_pos, view_pos, roomLight, objectColA0 + objectColA1, objectColD0 + objectColD1, objectColS0 + objectColS1);

	
	//resultant = roadLightSource;
	//resultant = col;
	//resultant = objectCol;
	//resultant = resultant*objectCol;
	color = vec4(resultant, 1.0f);
	//color = vec4(objectColA0*objectColA1+objectColD0* objectColD1+ objectColS0*objectColS1,1.0f) ;

	//color = objectColA0*objectColA1*objectColD0* objectColD1+;
	//color = vec4(texture(_texture, _texCoordinate));
	//color += vec4(resultant, 1.0f);
}



vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewPos, vec3 lightSource, vec3 ambientCoef, vec3 diffuseCoef, vec3 specularCoef)
{
	
	
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(viewPos - frag_pos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), alpha);
	// Combine results
	//ambient
	vec3 ambient = ambient_coef * ambientCoef * lightSource;
	//diffuse
	vec3 diffuse = diffuse_coef * diffuseCoef * diff * lightSource;
	//specular
	vec3 specular = specular_coef * specularCoef * spec * lightSource;
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (attenuation_const + attenuation_linear *distance + attenuation_quad * (distance * distance));
	// combine results
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewPos, vec3 lightSource, vec3 ambientCoef, vec3 diffuseCoef, vec3 specularCoef)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(viewPos - frag_pos);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), alpha);
	// Spotlight intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// Combine results
	vec3 ambient = ambient_coef * ambientCoef * lightSource;
	vec3 diffuse = diffuse_coef * diffuseCoef * diff * lightSource;
	vec3 specular = specular_coef * specularCoef * spec * lightSource;
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (attenuation_const + attenuation_linear *distance + attenuation_quad * (distance * distance));
	// combine results
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return (ambient + diffuse + specular);
}
