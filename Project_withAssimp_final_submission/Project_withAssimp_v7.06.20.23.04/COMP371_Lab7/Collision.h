#ifndef COLLISION_H
#define COLLISION_H

//#include "Camera.h"
#include "Model.h"
#include "Mesh.h"
#include <iostream>

using namespace std;

bool getDetect(glm::vec2 &pos, glm::vec2 &p1, glm::vec2 &p2, GLfloat &offset)
{
	glm::vec2 vec1, vec2;
	vec1 = pos - p1;
	vec2 = p2 - p1;

	// A dot B = ||A|| * ||B|| * cos(theta) 
	GLfloat vec1L = length(vec1), vec2L = length(vec2);
	//get angle
	GLfloat theta = acos(glm::dot(vec1, vec2) / (vec1L * vec2L));
	// check either left side miss or right side miss
	if ((theta > glm::radians(90.0f)) || ((vec1L * cos(theta)) > vec2L))
	{
		//cout << "nc 1" << endl;
		return false;
	}
	else
	{
		// check distance
		GLfloat distance = vec1L * sin(theta);

		if (distance < offset)
		{
			//cout << "c 1" << endl;

			return true;
		}

		else
		{
			//cout << "nc 1" << endl;

			return false;
		}

	}
}

bool CollisionDetectCAMtoWALL(glm::vec3 &position, GLfloat &offset, vector<glm::vec3> &vertex)
{
	bool collision;
	for (GLuint i = 0; i < vertex.size(); i += 3)
	{		

		glm::vec2 pointA = glm::vec2(vertex[i].x, -vertex[i].z);
		glm::vec2 pointB = glm::vec2(vertex[i + 1].x, -vertex[i + 1].z);
		glm::vec2 pointC = glm::vec2(vertex[i + 2].x, -vertex[i + 2].z);
		glm::vec2 pos = glm::vec2(position.x, -position.z);
		collision = getDetect(pos, pointA, pointB, offset);

		if (collision)
		{
			return true;
		}
		else {
			collision = getDetect(pos, pointB, pointC, offset);
			if (collision)
			{
				return true;
			}
			else
			{
				collision = getDetect(pos, pointC, pointA, offset);
				if (collision)
				{
					return true;
				}
				
			}
		}
	}
	return false;

}




bool CollisionDetectPOStoOBJ(glm::vec2 &position, GLfloat &offset, vector<glm::vec3> &BB)
{
		glm::vec2 pointA;
		glm::vec2 pointB;

		//bool collision;
		for (GLuint i = 0; i < BB.size(); i++)
		{

			pointA = glm::vec2(BB[i].x, -BB[i].z);
			pointB = glm::vec2(BB[(i + 1) % BB.size()].x, -BB[(i + 1) % BB.size()].z);
			//collision = getDetect(pos, pointA, pointB, offset);

			if (getDetect(position, pointA, pointB, offset))
			{
				return true;
			}

		}
	return false;

}

bool collisionDetectCAMtoOBJ(glm::vec3 &position, GLfloat &offset, vector<Model*> &models)
{
	vector<glm::vec3> BB;
	glm::vec2 pos = glm::vec2(position.x, -position.z);
	glm::vec2 centerP;

	for (GLuint i = 0; i < models.size(); i++)
	{
		if (models[i]->MOVABLE != 0)
		{
			BB = models[i]->BB;
			centerP = glm::vec2(models[i]->center.x, -models[i]->center.z);
			if ((length(pos - centerP) + offset) < (models[i]->size / 2.0f))
			{
				if (CollisionDetectPOStoOBJ(pos, offset, BB))
					return true;
			}
		}
	}
	return false;
}

bool CollisionDetectOBJtoWALL(Model &obj, GLfloat &offset, vector<glm::vec3> &vertex)
{
	bool collision;

	for (GLuint j = 0; j < obj.BB.size(); j++)
	{
		for (GLuint i = 0; i < vertex.size(); i += 3)
		{
			
			glm::vec2 pointA = glm::vec2(vertex[i].x, -vertex[i].z);
			glm::vec2 pointB = glm::vec2(vertex[i + 1].x, -vertex[i + 1].z);
			glm::vec2 pointC = glm::vec2(vertex[i + 2].x, -vertex[i + 2].z);
			glm::vec2 pos = glm::vec2(obj.BB[j].x, -obj.BB[j].z);
			collision = getDetect(pos, pointA, pointB, offset);

			if (collision)
			{
				return true;
			}
			else {
				collision = getDetect(pos, pointB, pointC, offset);
				if (collision)
				{
					return true;
				}
				else
				{
					collision = getDetect(pos, pointC, pointA, offset);
					if (collision)
					{
						return true;
					}

				}
			}
		}
	}
	return false;

}

bool collisionDetectOBJtoOBJ(GLuint &colID, GLfloat &offset, vector<Model*> &models)
{
	//vector<glm::vec3> BB = models[colID]->BB;
	glm::vec2 pos;
	GLuint cloeset;
	GLfloat shortest;
	for (GLuint i = 0; i < models.size(); i++)
	{
		if (models[i]->MOVABLE != 0 && i != colID)
		{
			if (length(models[colID]->bottomMid - models[i]->bottomMid) + offset < (models[colID]->size + models[i]->size) / 2.0f)
			{
				vector<GLfloat> distance;
				//test closest point of moving obj's BB towards the target obj's BB at obj id i 
				for (GLuint j = 0; j < models[colID]->BB.size(); j++)
				{
					distance.push_back( length(models[colID]->BB[j] - models[i]->bottomMid));
				}
				shortest = distance[0];
				cloeset = 0;
				for (GLuint j = 1; j < distance.size(); j++)
				{
					if (shortest > distance[j])
					{
						shortest = distance[j];
						cloeset = j;
					}
				}
				pos = glm::vec2(models[colID]->BB[cloeset].x, -models[colID]->BB[cloeset].z);
				if (CollisionDetectPOStoOBJ(pos, offset, models[i]->BB))
				{
					return true;
				}
				
				distance.clear();

				//opposite as above
				for (GLuint j = 0; j < models[i]->BB.size(); j++)
				{
					distance.push_back(length(models[i]->BB[j] - models[colID]->bottomMid));
				}
				shortest = distance[0];
				cloeset = 0;
				for (GLuint j = 1; j < distance.size(); j++)
				{
					if (shortest > distance[j])
					{
						shortest = distance[j];
						cloeset = j;
					}
				}
				pos = glm::vec2(models[i]->BB[cloeset].x, -models[i]->BB[cloeset].z);
				if (CollisionDetectPOStoOBJ(pos, offset, models[colID]->BB))
				{
					return true;
				}
			}
		}
	}
	return false;
}

#endif
