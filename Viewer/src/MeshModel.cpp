#include "MeshModel.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#define FACE_ELEMENTS 3


using namespace std;

// A struct for processing a single line in a wafefront obj file:
// https://en.wikipedia.org/wiki/Wavefront_.obj_file
struct FaceIdx
{
	// For each of the following 
	// Saves vertex indices
	int v[FACE_ELEMENTS];
	// Saves vertex normal indices
	int vn[FACE_ELEMENTS];
	// Saves vertex texture indices
	int vt[FACE_ELEMENTS];

	FaceIdx()
	{
		for (int i = 0; i < FACE_ELEMENTS + 1; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdx(std::istream& issLine)
	{
		for (int i = 0; i < FACE_ELEMENTS + 1; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for(int i = 0; i < FACE_ELEMENTS; i++)
		{
			issLine >> std::ws >> v[i] >> std::ws;
			if (issLine.peek() != '/')
			{
				continue;
			}
			issLine >> c >> std::ws;
			if (issLine.peek() == '/')
			{
				issLine >> c >> std::ws >> vn[i];
				continue;
			}
			else
			{
				issLine >> vt[i];
			}
			if (issLine.peek() != '/')
			{
				continue;
			}
			issLine >> c >> vn[i];
		}
	}
};

glm::vec3 vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return glm::vec3(x, y, z);
}

glm::vec2 vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return glm::vec2(x, y);
}

MeshModel::MeshModel(const string& fileName)
{
	LoadFile(fileName);
}

MeshModel::~MeshModel()
{
	delete[] vertexPositions;
}

void MeshModel::SetWorldTransform(glm::mat4x4 & transformation)
{
	worldTransform = transformation;
}

const glm::mat4x4& MeshModel::GetWorldTransform()
{
	return worldTransform;
}

void MeshModel::SetNormalTransform(glm::mat4x4 & transformation)
{
	normalTransform = transformation;
}

const glm::mat4x4 & MeshModel::GetNormalTransform()
{
	return normalTransform;
}

void MeshModel::LoadFile(const string& fileName)
{
	ifstream ifile(fileName.c_str());
	vector<FaceIdx> faces;
	vector<glm::vec3> vertices;
	// while not end of file
	while (!ifile.eof())
	{
		// get line
		string curLine;
		getline(ifile, curLine);

		// read the type of the line
		istringstream issLine(curLine);
		string lineType;

		issLine >> std::ws >> lineType;

		// based on the type parse data
		if (lineType == "v")
		{
			vertices.push_back(vec3fFromStream(issLine));
		}
		else if (lineType == "f")
		{
			faces.push_back(issLine);
		}
		else if (lineType == "#" || lineType == "")
		{
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"";
		}
	}
	//Vertex_positions is an array of vec3. Every three elements define a triangle in 3D.
	//If the face part of the obj is
	//f 1 2 3
	//f 1 3 4
	//Then vertexPositions should contain:
	//vertexPositions={v1,v2,v3,v1,v3,v4}

	vertexPosSize = faces.size()*FACE_ELEMENTS;
	vertexPositions = new glm::vec3[vertexPosSize];

	// iterate through all stored faces and create triangles
	size_t posIdx = 0;
	for each (FaceIdx faces in faces)
	{
		for (int i = 0; i < FACE_ELEMENTS; i++)
		{
			int currentVertexIdx = faces.v[i];
			float x = vertices[currentVertexIdx - 1].x;
			float y = vertices[currentVertexIdx - 1].y;
			float z = vertices[currentVertexIdx - 1].z;
			vertexPositions[posIdx++] = glm::vec3(x, y, z);
		}
	}
}

const vector<glm::vec3>* MeshModel::Draw()
{
	vector<glm::vec3>* meshModel = new vector<glm::vec3>();
	for (size_t i = 0; i < vertexPosSize; i++)
	{
		meshModel->push_back(vertexPositions[i]);
	}
	return meshModel;
}
