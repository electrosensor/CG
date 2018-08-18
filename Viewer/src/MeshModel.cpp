#include "MeshModel.h"
#include "Util.h"
#include "Defs.h"

using namespace std;
using namespace glm;


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

vec3 vec3fFromStream(std::istream& issLine)
{
	float x, y, z;
	issLine >> x >> std::ws >> y >> std::ws >> z;
	return vec3(x, y, z);
}

vec2 vec2fFromStream(std::istream& issLine)
{
	float x, y;
	issLine >> x >> std::ws >> y;
	return vec2(x, y);
}

MeshModel::MeshModel(const string& fileName) : m_modelTransformation(I_MATRIX),
											   m_normalTransformation(I_MATRIX),
                                               m_worldTransformation(I_MATRIX),
                                               m_modelCentroid(HOMOGENEOUS_VECTOR4)
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

void MeshModel::SetWorldTransformation(mat4x4 & transformation)
{
	m_worldTransformation = transformation;
}

const mat4x4& MeshModel::GetWorldTransformation()
{
	return m_worldTransformation;
}

void MeshModel::SetNormalTransformation(mat4x4 & transformation)
{
	m_normalTransformation = transformation;
}

const mat4x4 & MeshModel::GetNormalTransformation()
{
	return m_normalTransformation;
}

void MeshModel::SetModelTransformation(mat4x4& transformation)
{
    m_modelTransformation = transformation;
}

const mat4x4& MeshModel::GetModelTransformation()
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
	vector<vec4> vertices;
    vector<vec4> normals;

    vec4 maxCoords = { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() , 1 };
    vec4 minCoords = {  std::numeric_limits<float>::max(),  std::numeric_limits<float>::max(),  std::numeric_limits<float>::max() , 1 };
    vec4 normalizedVec = HOMOGENEOUS_VECTOR4;
    unsigned int numVertices = 0;
    vec4 modelCentroid = HOMOGENEOUS_VECTOR4;
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
            vec4 parsedVec = Util::toHomogeneousForm(vec3fFromStream(issLine));
            
            minCoords.x = (minCoords.x > parsedVec.x) ? parsedVec.x : minCoords.x;
            minCoords.y = (minCoords.y > parsedVec.y) ? parsedVec.y : minCoords.y;
            minCoords.z = (minCoords.z > parsedVec.z) ? parsedVec.z : minCoords.z;

            maxCoords.x = (maxCoords.x < parsedVec.x) ? parsedVec.x : maxCoords.x;
            maxCoords.y = (maxCoords.y < parsedVec.y) ? parsedVec.y : maxCoords.y;
            maxCoords.z = (maxCoords.z < parsedVec.z) ? parsedVec.z : maxCoords.z;
           
            m_modelCentroid += parsedVec;
            
            numVertices++;

			vertices.push_back(parsedVec);
		}
        else if (lineType == "vn")
        {
            normals.push_back(Util::toHomogeneousForm(vec3fFromStream(issLine)));
        }
		else if (lineType == "f")
		{
			faces.push_back(issLine);
		}
		else if (lineType == "#" || lineType.empty())
		{
			// comment / empty line
		}
		else
		{
			cout << "Found unknown line Type \"" << lineType << "\"\n";
		}
	}

    m_modelCentroid /= (float)numVertices;
    minCoords -= m_modelCentroid;
    maxCoords -= m_modelCentroid;


    float totalMin = MIN(minCoords.x, MIN(minCoords.y, minCoords.z));
    float totalMax = MAX(maxCoords.x, MAX(maxCoords.y, maxCoords.z));


    m_verticesSize    = vertices.size();
    m_vertices        = new vec4[m_verticesSize];
	m_vertexPosSize   = faces.size()*FACE_ELEMENTS;
	m_vertexPositions = new vec4[m_vertexPosSize];
    m_vertexNormSize  = normals.size();
    m_vertexNormals   = new vec4[m_vertexNormSize];



    for (unsigned int i = 0; i < m_verticesSize; i++)

    {

        normalizedVec.x = NORMALIZE_COORDS((vertices[i].x - m_modelCentroid.x), totalMin, totalMax);
        normalizedVec.y = NORMALIZE_COORDS((vertices[i].y - m_modelCentroid.y), totalMin, totalMax);
        normalizedVec.z = NORMALIZE_COORDS((vertices[i].z - m_modelCentroid.z), totalMin, totalMax);
                                           
       // fprintf(stderr, "x = %f, y = %f, z = %f\n", normalizedVec.x, normalizedVec.y, normalizedVec.z);

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
			m_vertexPositions[posIdx++] = vec4(x, y, z , 1);
		}
	}
    
    for (unsigned int i = 0; i < m_vertexNormSize; i++)
    {
        m_vertexNormals[i] = normals[i];
    }
    
    m_modelCentroid.x = NORMALIZE_COORDS(modelCentroid.x , totalMin, totalMax);
    m_modelCentroid.y = NORMALIZE_COORDS(modelCentroid.y , totalMin, totalMax);
    m_modelCentroid.z = NORMALIZE_COORDS(modelCentroid.z , totalMin, totalMax);

    m_minCoords.x     = NORMALIZE_COORDS(minCoords.x     , totalMin, totalMax);
    m_minCoords.y     = NORMALIZE_COORDS(minCoords.y     , totalMin, totalMax);
    m_minCoords.z     = NORMALIZE_COORDS(minCoords.z     , totalMin, totalMax);

    m_maxCoords.x     = NORMALIZE_COORDS(maxCoords.x     , totalMin, totalMax);
    m_maxCoords.y     = NORMALIZE_COORDS(maxCoords.y     , totalMin, totalMax);
    m_maxCoords.z     = NORMALIZE_COORDS(maxCoords.z     , totalMin, totalMax);


}

void MeshModel::Draw(tuple<vector<vec4>, vector<vec4>, vector<vec4> >& modelData)
{
	for (size_t i = 0; i < m_vertexPosSize; i++)
	{
        get<TUPLE_POSITIONS>(modelData).push_back(m_vertexPositions[i]);
	}
   
    for (size_t i = 0; i < m_verticesSize; i++)
    {
        get<TUPLE_VERTICES>(modelData).push_back(m_vertices[i]);
    }

    for (size_t i = 0; i < m_vertexNormSize; i++)
    {
        get<TUPLE_VNORMALS>(modelData).push_back(m_vertexNormals[i]);
    }
};

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

void CamMeshModel::Draw(tuple<vector<vec4>, vector<vec4>, vector<vec4> >& modelData)
{
    vector<vec4> camModelVertices;

    for (size_t i = 0; i < m_vertexPosSize; i++)
    {
        vec4 vertex = mat4x4(SCALING_MATRIX4(0.3f))*m_vertexPositions[i];
        camModelVertices.push_back(vertex);
    }

    get<TUPLE_POSITIONS>(modelData) = camModelVertices;
}
