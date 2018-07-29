#include "MeshModel.h"
#include "Util.h"
#include "Defs.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


typedef Model* PModel;
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

MeshModel::MeshModel(const string& fileName) : m_worldTransformation(I_MATRIX), 
											   m_normalTransformation(I_MATRIX)
{

	glm::mat4x4 scale = SCALING_MATRIX(7);
	m_worldTransformation = m_worldTransformation * scale;

	LoadFile(fileName);
}

MeshModel::~MeshModel()
{
	delete[] m_vertexPositions;
}

void MeshModel::SetWorldTransformation(glm::mat4x4 & transformation)
{
	m_worldTransformation = transformation;
}

const glm::mat4x4& MeshModel::GetWorldTransformation()
{
	return m_worldTransformation;
}

void MeshModel::SetNormalTransformation(glm::mat4x4 & transformation)
{
	m_normalTransformation = transformation;
}

const glm::mat4x4 & MeshModel::GetNormalTransformation()
{
	return m_normalTransformation;
}

void MeshModel::LoadFile(const string& fileName)
{
	ifstream ifile(fileName.c_str());

	if (ifile.fail())
	{
		fprintf(stderr, "Opening file %s failed, goodbye cruel world - harakiri!!!", fileName.c_str());
		exit(RC_IO_ERROR);
	}

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

	m_vertexPosSize = faces.size()*FACE_ELEMENTS;
	m_vertexPositions = new glm::vec3[m_vertexPosSize];

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
			m_vertexPositions[posIdx++] = glm::vec3(x, y, z);
		}
	}
}

const vector<glm::vec3>* MeshModel::Draw()
{
	vector<glm::vec3>* meshModelVertices = new vector<glm::vec3>();
	for (size_t i = 0; i < m_vertexPosSize; i++)
	{
		glm::vec3 vertex = m_vertexPositions[i];
		vertex = Util::toNormalForm(m_normalTransformation * m_worldTransformation * Util::toHomogeniousForm(vertex)); //TODO_YURI: check the order of transformations
		meshModelVertices->push_back(vertex);
	}
	return meshModelVertices;
}

string* PrimMeshModel::setPrimModelFilePath(PRIM_MODEL primModel)
{
	switch (primModel)
	{
	case PM_SPHERE:
		m_pPrimModelString = new string("ObjFiles/teddy.obj");
		break;
	case PM_BOX:
		m_pPrimModelString = new string("ObjFiles/cow-nonormals.obj");
		break;
	default:
		m_pPrimModelString = new string("NULL");
		break;
	}
	return m_pPrimModelString;
}
