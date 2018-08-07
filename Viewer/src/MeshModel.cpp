#include "MeshModel.h"
#include "Util.h"
#include "Defs.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


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

MeshModel::MeshModel(const string& fileName) : m_modelTransformation(I_MATRIX),
											   m_normalTransformation(I_MATRIX),
                                               m_worldTransformation(I_MATRIX)
{
	LoadFile(fileName);
    setModelRenderingState(true);
    buildBorderCube(m_cubeLines);
}

MeshModel::~MeshModel()
{
    delete[] m_vertices;
	delete[] m_vertexPositions;
    delete[] m_vertexNormals;
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

void MeshModel::SetModelTransformation(glm::mat4x4& transformation)
{
    m_modelTransformation = transformation;
}

const glm::mat4x4& MeshModel::GetModelTransformation()
{
    return m_modelTransformation;
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
    vector<glm::vec3> normals;

    glm::vec3 maxCoords = { std::numeric_limits<float>::min(), std::numeric_limits<float>::min() ,std::numeric_limits<float>::min() };
    glm::vec3 minCoords = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    glm::vec3 normalizedVec;
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
            glm::vec3 parsedVec = vec3fFromStream(issLine);
            
            minCoords.x = (minCoords.x > parsedVec.x) ? parsedVec.x : minCoords.x;
            minCoords.y = (minCoords.y > parsedVec.y) ? parsedVec.y : minCoords.y;
            minCoords.z = (minCoords.z > parsedVec.z) ? parsedVec.z : minCoords.z;

            maxCoords.x = (maxCoords.x < parsedVec.x) ? parsedVec.x : maxCoords.x;
            maxCoords.y = (maxCoords.y < parsedVec.y) ? parsedVec.y : maxCoords.y;
            maxCoords.z = (maxCoords.z < parsedVec.z) ? parsedVec.z : maxCoords.z;

			vertices.push_back(parsedVec);
		}
        else if (lineType == "vn")
        {
            normals.push_back(vec3fFromStream(issLine));
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

    float totalMin = MIN(minCoords.x, MIN(minCoords.y, minCoords.z));
    float totalMax = MAX(maxCoords.x, MAX(maxCoords.y, maxCoords.z));

    m_verticesSize    = vertices.size();
    m_vertices        = new glm::vec3[m_verticesSize];
	m_vertexPosSize   = faces.size()*FACE_ELEMENTS;
	m_vertexPositions = new glm::vec3[m_vertexPosSize];
    m_vertexNormSize  = normals.size();
    m_vertexNormals   = new glm::vec3[m_vertexNormSize];



    for (unsigned int i = 0; i < m_verticesSize; i++)

    {
        normalizedVec.x = NORMALIZE_COORDS(vertices[i].x, totalMin, totalMax);
        normalizedVec.y = NORMALIZE_COORDS(vertices[i].y, totalMin, totalMax);
        normalizedVec.z = NORMALIZE_COORDS(vertices[i].z, totalMin, totalMax);

        fprintf(stderr, "x = %f, y = %f, z = %f\n", normalizedVec.x, normalizedVec.y, normalizedVec.z);

        m_vertices[i] = normalizedVec;

    }

	// iterate through all stored faces and create triangles
	size_t posIdx = 0;
	for each (FaceIdx face in faces)
	{
		for (int i = 0; i < FACE_ELEMENTS; i++)
		{
			int currentVertexIdx = face.v[i];
            float x = m_vertices[currentVertexIdx - 1].x;
            float y = m_vertices[currentVertexIdx - 1].y;
            float z = m_vertices[currentVertexIdx - 1].z;
			m_vertexPositions[posIdx++] = glm::vec3(x, y, z);
		}
	}
    
    for (unsigned int i = 0; i < m_vertexNormSize; i++)
    {
        normalizedVec.x = NORMALIZE_COORDS(normals[i].x, totalMin, totalMax);
        normalizedVec.y = NORMALIZE_COORDS(normals[i].y, totalMin, totalMax);
        normalizedVec.z = NORMALIZE_COORDS(normals[i].z, totalMin, totalMax);

        m_vertexNormals[i] = normalizedVec;
    }

    m_minCoords.x = NORMALIZE_COORDS(minCoords.x, totalMin, totalMax);
    m_minCoords.y = NORMALIZE_COORDS(minCoords.y, totalMin, totalMax);
    m_minCoords.z = NORMALIZE_COORDS(minCoords.z, totalMin, totalMax);

    m_maxCoords.x = NORMALIZE_COORDS(maxCoords.x, totalMin, totalMax);
    m_maxCoords.y = NORMALIZE_COORDS(maxCoords.y, totalMin, totalMax);
    m_maxCoords.z = NORMALIZE_COORDS(maxCoords.z, totalMin, totalMax);


}

std::pair<std::vector<glm::vec3>, std::pair<std::vector<glm::vec3>, std::vector<glm::vec3> > >* MeshModel::Draw()
{
    std::pair<std::vector<glm::vec3>, std::pair<std::vector<glm::vec3>, std::vector<glm::vec3> > >* verticesData = new std::pair<std::vector<glm::vec3>, std::pair<std::vector<glm::vec3>, std::vector<glm::vec3> > >();
    vector<glm::vec3> meshModelVertexPositions;
    vector<glm::vec3> meshModelVertices;
    vector<glm::vec3> meshModelVerticesNormals;

	for (size_t i = 0; i < m_vertexPosSize; i++)
	{
		glm::vec3 vertexPosition = m_vertexPositions[i];
        vertexPosition = Util::toCartesianForm(m_worldTransformation * m_modelTransformation * Util::toHomogeneousForm(vertexPosition)); //TODO_YURI: check the order of transformations
        meshModelVertexPositions.push_back(vertexPosition);
	}
   
    for (size_t i = 0; i < m_verticesSize; i++)
    {
        glm::vec3 vertex = m_vertices[i];
        vertex = Util::toCartesianForm(m_worldTransformation * m_modelTransformation * Util::toHomogeneousForm(vertex)); //TODO_YURI: check the order of transformations
        meshModelVertices.push_back(vertex);
    }


    for (size_t i = 0; i < m_vertexNormSize; i++)
    {
        glm::vec3 normal = m_vertexNormals[i];
        normal = Util::toCartesianForm(m_worldTransformation * m_normalTransformation * Util::toHomogeneousForm(normal)); //TODO_YURI: check the order of transformations
        meshModelVerticesNormals.push_back(normal);
    }

    verticesData->first         = meshModelVertexPositions;
    verticesData->second.first  = meshModelVertices;
    verticesData->second.second = meshModelVerticesNormals;

	return verticesData;
}

string* PrimMeshModel::setPrimModelFilePath(PRIM_MODEL primModel)
{
	switch (primModel)
	{
	case PM_SPHERE:
		m_pPrimModelString = new string("PrimModels/PM_Sphere.obj");
		break;
	case PM_CUBE:
		m_pPrimModelString = new string("PrimModels/PM_Cube.obj");
		break;
	default:
		m_pPrimModelString = new string("NULL");
		break;
	}
	return m_pPrimModelString;
}

