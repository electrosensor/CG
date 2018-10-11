#include "MeshModel.h"
#include "lodepng.h"
#include "lodepng_util.h"

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

MeshModel::MeshModel(const std::string& fileName, const Surface& material, GLuint program) : m_modelTransformation(SCALING_MATRIX4(0.5f)),
                                               m_scaleTransformation(I_MATRIX),
                                               m_translateTransformation(I_MATRIX),
                                               m_rotateTransformation(I_MATRIX),
											   m_normalTransformation(I_MATRIX),
                                               m_worldTransformation(I_MATRIX),
                                               m_modelCentroid(ZERO_VEC3),
                                               m_surface(material)

{
    m_cur_prog = program;
    m_tex_height = 0;
    m_tex_width = 0;
    m_tex_data = nullptr;
	LoadFile(fileName, program);
    setModelRenderingState(true);
    buildBorderCube(m_cubeLines);
}

MeshModel::~MeshModel()
{

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    if (TEX != 0)
    {
        glDeleteTextures(1, &TEX);
        TEX = 0;
    }

    delete[] m_vertices;
	delete[] m_polygons;
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

void MeshModel::SetScaleTransformation(glm::mat4x4& transformation)
{
    m_scaleTransformation = transformation;
}

void MeshModel::SetTranslateTransformation(glm::mat4x4& transformation)
{
    m_translateTransformation = transformation;
}

void MeshModel::SetRotateTransformation(glm::mat4x4& transformation)
{
    m_rotateTransformation = transformation;
}

const mat4x4& MeshModel::GetModelTransformation()
{
    return m_modelTransformation;
}

const glm::mat4x4& MeshModel::GetScaleTransformation()
{
    return m_scaleTransformation;
}

const glm::mat4x4& MeshModel::GetTranslateTransformation()
{
    return m_translateTransformation;
}

const glm::mat4x4& MeshModel::GetRotateTransformation()
{
    return m_rotateTransformation;
}

void MeshModel::LoadFile(const std::string& fileName, GLuint program)
{
	ifstream ifile(fileName.c_str());

	if (ifile.fail())
	{
		fprintf(stderr, "Opening file %s failed, goodbye cruel world - harakiri!!!", fileName.c_str());
		exit(RC_IO_ERROR);
	}

	vector<FaceIdx> faces;
	vector<vec3> vertices;
    vector<vec3> normals;

    vec3 maxCoords = { -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };
    vec3 minCoords = {  std::numeric_limits<float>::infinity(),  std::numeric_limits<float>::infinity(),  std::numeric_limits<float>::infinity() };
    vec3 normalizedVec = ZERO_VEC3;
    unsigned int numVertices = 0;
    vec3 modelCentroid = ZERO_VEC3;
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
            vec3 parsedVec = vec3fFromStream(issLine);
            
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
            normals.push_back(vec3fFromStream(issLine));
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
    m_vertices        = new vec3[m_verticesSize];
	m_polygonsSize    = faces.size();
    m_polygons        = new Face[m_polygonsSize];
    m_verticesNormSize  = normals.size();
    m_vertexNormals   = new vec3[m_verticesNormSize];
    m_vPositionsSize = m_polygonsSize*FACE_ELEMENTS;
    m_vertexPositions = new vec3[m_vPositionsSize];

    for (unsigned int i = 0; i < m_verticesSize; i++)

    {
        normalizedVec.x = NORMALIZE_COORDS((vertices[i].x - m_modelCentroid.x), totalMin, totalMax);
        normalizedVec.y = NORMALIZE_COORDS((vertices[i].y - m_modelCentroid.y), totalMin, totalMax);
        normalizedVec.z = NORMALIZE_COORDS((vertices[i].z - m_modelCentroid.z), totalMin, totalMax);
                                           
//         fprintf(stderr, "x = %f, y = %f, z = %f\n", normalizedVec.x, normalizedVec.y, normalizedVec.z);
        m_vertices[i] = normalizedVec;
    }
    
    for (unsigned int i = 0; i < m_verticesNormSize; i++)
    {
        m_vertexNormals[i] = normals[i];
    }

	// iterate through all stored faces and create triangles
	size_t posIdx = 0;
    int j = 0;
	for each (FaceIdx face in faces)
	{
        pair<vec3, vec3> currentFace[FACE_ELEMENTS];
		for (int i = 0; i < FACE_ELEMENTS; i++)
		{
			int currentVertexIdx = face.v[i];
            float x = m_vertices[currentVertexIdx - 1].x;
            float y = m_vertices[currentVertexIdx - 1].y;
            float z = m_vertices[currentVertexIdx - 1].z;
            float vn_x = (currentVertexIdx < m_verticesNormSize) ? m_vertexNormals[currentVertexIdx - 1].x : 0.f;
            float vn_y = (currentVertexIdx < m_verticesNormSize) ? m_vertexNormals[currentVertexIdx - 1].y : 0.f;
            float vn_z = (currentVertexIdx < m_verticesNormSize) ? m_vertexNormals[currentVertexIdx - 1].z : 0.f;
            currentFace[i] = { vec3(x, y, z), vec3(vn_x, vn_y, vn_z) };
            m_vertexPositions[j + i] = vec3(x, y, z);
		}
        j+=3;

        auto nrm1_3 = currentFace[0].first;
        auto nrm2_3 = currentFace[1].first;
        auto nrm3_3 = currentFace[2].first;

        auto subs1 = nrm3_3 - nrm1_3;
        auto subs2 = nrm2_3 - nrm1_3;

        auto faceNormal = cross(subs1, subs2);

        auto faceCenter = (nrm1_3 + nrm2_3 + nrm3_3) / 3.0f;

        auto normalizedFaceNormal = Util::isVecEqual(faceNormal, vec3(0)) ? faceNormal : normalize(faceNormal);

        Face currentPolygon(currentFace[0].first, currentFace[1].first, currentFace[2].first, faceCenter, normalizedFaceNormal, &m_surface, currentFace[0].second, currentFace[1].second, currentFace[2].second);
        m_polygons[posIdx++] = currentPolygon;
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


    ////////////////

    unsigned VerticesPositionsSize = m_vPositionsSize * 3;
    GLfloat* VerticesPositions = new GLfloat[VerticesPositionsSize];
    for (size_t i = 0, j = 0; j < m_vPositionsSize; i += 3, j++)
    {
        VerticesPositions[i] = (GLfloat)m_vertexPositions[j].x;
        VerticesPositions[i + 1] = (GLfloat)m_vertexPositions[j].y;
        VerticesPositions[i + 2] = (GLfloat)m_vertexPositions[j].z;

    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VerticesPositions[0]) * VerticesPositionsSize, VerticesPositions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VerticesPositions[0]) * 3, nullptr);
    glEnableVertexAttribArray(0);

// 
//     unsigned VerticesColorsSize = VerticesPositionsSize;
//     GLfloat* VerticesColors = new GLfloat[VerticesPositionsSize];
//     for (size_t i = 0, j = 0; j < VerticesPositionsSize && i < VerticesPositionsSize; i += 3, j++)
//     {
//         VerticesColors[i] =     (GLfloat)(m_surface.m_ambientColor.x);
//         VerticesColors[i + 1] = (GLfloat)(m_surface.m_ambientColor.y);
//         VerticesColors[i + 2] = (GLfloat)(m_surface.m_ambientColor.z);
//     }
// 

//     glBindTexture(GL_COLOR_BUFFER_BIT, TEX);


    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VerticesPositions[0]) * 3, (void*)(sizeof(VerticesPositions[0])*1));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void MeshModel::Draw(std::tuple<std::vector<Face>, std::vector<glm::vec3>, std::vector<glm::vec3>, std::vector<glm::vec3> >& modelData)
{


    if (m_tex_data) {
        glActiveTexture(GL_TEXTURE0);
//         glProgramUniform1i( m_cur_prog, glGetUniformLocation(m_cur_prog, "textureSampler"), TEX);
        glBindTexture(GL_TEXTURE_2D, TEX);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glDrawArrays(GL_TRIANGLES, 0, m_vPositionsSize * 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);



	for (size_t i = 0; i < m_polygonsSize; i++)
	{
     
        get<TUPLE_POLYGONS>(modelData).push_back(m_polygons[i]);
	}
   
    for (size_t i = 0; i < m_verticesSize; i++)
    {
        get<TUPLE_VERTICES>(modelData).push_back(m_vertices[i]);
    }

    for (size_t i = 0; i < m_verticesNormSize; i++)
    {
        get<TUPLE_VNORMALS>(modelData).push_back(m_vertexNormals[i]);
    }

    for (size_t i = 0; i < m_vPositionsSize; i++)
    {
        get<TUPLE_VPOSITIONS>(modelData).push_back(m_vertexPositions[i]);
    }

//     for (size_t i = 0; i < m_vPositionsSize; i++)
//     {
//         get<TUPLE_TEXTURES>(modelData).push_back(m_vertexPositions[i]);
//     }
};

void MeshModel::ApplyTexture(std::string path)
{
   lodepng_decode_file(&m_tex_data, &m_tex_width, &m_tex_height, path.c_str(), LCT_RGBA, 8);
   if (m_tex_data)
   {
       glGenTextures(1, &TEX);

       glBindTexture(GL_TEXTURE_2D, TEX);
       glActiveTexture(GL_TEXTURE0);


       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_tex_width, m_tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_data);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

       glGenerateMipmap(GL_TEXTURE_2D);


       //glBindTexture(GL_TEXTURE_2D, 0);


// 
//        free(m_tex_data);
//        m_tex_data = nullptr;
   }
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
    case PM_TEAPOT:
        m_pPrimModelString = new string("PrimModels/teapot.obj");
        break;
	default:
		m_pPrimModelString = new string("NULL");
		break;
	}
	return m_pPrimModelString;
}

void CamMeshModel::Draw(std::tuple<std::vector<Face>, std::vector<glm::vec3>, std::vector<glm::vec3>, std::vector<glm::vec3> >& modelData)
{



    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glDrawArrays(GL_TRIANGLES, 0, m_vPositionsSize * 3);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
