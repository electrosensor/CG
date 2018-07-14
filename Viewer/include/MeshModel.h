#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Model.h"

using namespace std;

/*
 * MeshModel class. Mesh model object represents a triangle mesh (loaded fron an obj file).
 * 
 */
class MeshModel : public Model
{
protected :
	//MeshModel() {}
	glm::vec3 *vertexPositions;
	// Add more attributes.
	glm::mat4x4 worldTransform;
	glm::mat4x4 normalTransform;

public:
	MeshModel(const string& fileName);
	~MeshModel();
	void LoadFile(const string& fileName);
	const vector<glm::vec3>* Draw();

	// Bresenham algorithm: 
	//TODO find right place
	inline void DrawLine(glm::vec2 p1, glm::vec2 p2) {
		int a = (p2.y - p1.y) / (p2.x - p1.x);
		if (a <= 0 || a >= 1) {

		}
		int c = p1.y + a * p1.x;
		int e = -1;
		int x = p1.x;
		int y = p1.y;
		while (x < p2.x) {
			e = 2*a*x + 2*c - 2*y - 1;
			if (e > 0) {
				y++;
				e -= 2;
			}
			//TurnOn(x, y); implement
			x++; 
			e += 2*a;
		}
	}
};
