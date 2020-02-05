//modified from https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h


#ifndef MODELLOADER_H
#define MODELLOADER_H


#include <../soil/SOIL.h>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include "Mesh.h"



using namespace std;

static	unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
static glm::vec3 ColorGen(const GLuint col_id);


class Model
{
public:
	/*  Model Data */
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<Mesh> meshes;
	string directory;
	bool gammaCorrection;
	GLuint COLOR_ID;
	glm::vec3 refCol;
	glm::mat4 objMM = glm::mat4(1.0f);
	GLuint MOVABLE;
	vector<glm::vec3> BB;
	glm::vec3 max;
	glm::vec3 min;
	glm::vec3 center;
	glm::vec3 bottomMid;
	GLfloat size;


	/*  Functions   */
	// constructor, expects a filepath to a 3D model.
	Model(string const &path, const GLuint _colorID, const GLuint _movable, bool gamma = false) : gammaCorrection(gamma)
	{
		COLOR_ID = _colorID;
		refCol = ColorGen(COLOR_ID);

		MOVABLE = _movable;
		loadModel(path);
		if(MOVABLE !=0)
			setCoords();
	}

	//bounding box update function
	void BBupdate(glm::mat4 &MM, bool backup)
	{
		if (backup)
		{
			for (GLuint i = 0; i < BB.size(); i++)
				BB[i] = glm::vec3(glm::inverse(MM) * glm::vec4(BB[i], 1.0f));
			center = glm::vec3(glm::inverse(MM) * glm::vec4(center, 1.0f));
			bottomMid = glm::vec3(glm::inverse(MM) * glm::vec4(bottomMid, 1.0f));

		}
		else
		{
			for (GLuint i = 0; i < BB.size(); i++)
				BB[i] = glm::vec3(MM * glm::vec4(BB[i], 1.0f));
			center = glm::vec3((MM) * glm::vec4(center, 1.0f));
			bottomMid = glm::vec3((MM) * glm::vec4(bottomMid, 1.0f));
		}
	}

	// draws the model, and thus all its meshes
	void Draw(Shader &shader)
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].Draw(shader, objMM);
	}
	// draw model in colid mode with related color
	void DrawColID(Shader &shader, const glm::vec3 &COLID)
	{

		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].DrawColID(shader, objMM, COLID);
	}
	


private:
	/*  Functions   */

	// setup center point and bottom-mid point and bounding box
	void setCoords()
	{
		max = min = meshes[0].vertices[0].Position;

		for (GLuint j = 0; j < meshes.size(); j++)
		{
			//seeking max & min value from the position vector
			for (GLuint i = 0; i <meshes[j].vertices.size(); i++)
			{
				(max.x >= meshes[j].vertices[i].Position.x) ? (max.x = max.x) : (max.x = meshes[j].vertices[i].Position.x);
				(max.y >= meshes[j].vertices[i].Position.y) ? (max.y = max.y) : (max.y = meshes[j].vertices[i].Position.y);
				(max.z >= meshes[j].vertices[i].Position.z) ? (max.z = max.z) : (max.z = meshes[j].vertices[i].Position.z);
				(min.x <= meshes[j].vertices[i].Position.x) ? (min.x = min.x) : (min.x = meshes[j].vertices[i].Position.x);
				(min.y <= meshes[j].vertices[i].Position.y) ? (min.y = min.y) : (min.y = meshes[j].vertices[i].Position.y);
				(min.z <= meshes[j].vertices[i].Position.z) ? (min.z = min.z) : (min.z = meshes[j].vertices[i].Position.z);

			}
		}
		//center of the obj
		center = (max + min) / 2.0f;
		//bottom-mid value of the obj
		bottomMid = glm::vec3(center.x, min.y, center.z);

		size = length(glm::vec3(max.x, 0.0f, max.z) - glm::vec3(min.x, 0.0f, min.z));

		// in 2 D
		//min-----------
		//   |\        |
		//   |  \      |
		//   |    \    |
		//   |      \  |
		//   |        \|
		//   ----------- max
		BB.push_back(glm::vec3(max.x, 0.0f, max.z));
		BB.push_back(glm::vec3(max.x, 0.0f, min.z));
		BB.push_back(glm::vec3(min.x, 0.0f, min.z));
		BB.push_back(glm::vec3(min.x, 0.0f, max.z));
	}


	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const &path)
	{
		// read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// retrieve the directory path of the filepath
		directory = path.substr(0, path.find_last_of('/'));

		// process ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode *node, const aiScene *scene)
	{
		// process each mesh located at the current node
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			// the node object only contains indices to index the actual objects in the scene. 
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			//aiUVTransform* UVscale = scene->mMaterials[i].mScaling;
			meshes.push_back(processMesh(mesh, scene));
		}
		// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}

	}

	Mesh processMesh(aiMesh *mesh, const aiScene *scene/*, aiUVTransform*/ )
	{
		// data to fill
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		// Walk through each of the mesh's vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
			
			if (mesh->mVertices)
			{				  // positions
				vector.x = mesh->mVertices[i].x;
				vector.y = mesh->mVertices[i].y;
				vector.z = mesh->mVertices[i].z;
				vertex.Position = vector;
			}

			if (mesh->mNormals)
			{
				// normals
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}
			// texture coordinates
			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				glm::vec2 UVScale;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x ;
				vec.y = mesh->mTextureCoords[0][i].y;
				//UVScale = mesh->;
				//aiUVTransform UVscale = ;

				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);

			if (mesh->mTangents)
			{
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
			}
			if (mesh->mBitangents)
			{
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
				vertices.push_back(vertex);
			}
		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];


			// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// diffuse: texture_diffuseN
			// specular: texture_specularN
			// normal: texture_normalN

			// 1. diffuse maps
			vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			// 4. ambient maps
			std::vector<Texture> ambientMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
			textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
		}

		// return a mesh object created from the extracted mesh data
		return Mesh(vertices, indices, textures);
	}

	// checks all material textures of a given type and loads the textures if they're not loaded yet.
	// the required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // if texture hasn't been loaded already, load it
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}
};


static unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = SOIL_load_image(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SOIL_free_image_data(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		SOIL_free_image_data(data);
	}

	return textureID;
}

// generate color based on color id
static glm::vec3 ColorGen(const GLuint col_id) {
	GLuint r, g = 0, b = 0;
	r = col_id % 255;
	return glm::vec3((GLfloat)r / 255.0f, (GLfloat)g / 255.0f, (GLfloat)b / 255.0f);
}





#endif