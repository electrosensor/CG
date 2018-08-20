#include <algorithm>
#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include "Util.h"
#include "Defs.h"
#include "glm/common.hpp"

using namespace std;
using namespace glm;

Renderer::Renderer() : m_width(DEFAULT_WIDTH), m_height(DEFAULT_HEIGHT)
{
    initOpenGLRendering();
    createBuffers(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Renderer::Renderer(int w, int h) : m_width(w), m_height(h), m_normalTransform(I_MATRIX), m_cameraTransform(I_MATRIX), m_objectTransform(I_MATRIX), m_cameraProjection(I_MATRIX), m_worldTransformation(I_MATRIX), m_bgColor(Util::getColor(CLEAR)), m_polygonColor(Util::getColor(BLACK)), m_wireframeColor(Util::getColor(WHITE))
{
    initOpenGLRendering();
    createBuffers(w, h);
}

Renderer::~Renderer()
{
    delete[] colorBuffer;
    delete[] zBuffer;
}


vec4 Renderer::processPipeline(const vec4& point, PIPE_TYPE pipeType /*= FULL*/)
{
    glm::vec4 piped;

    switch (pipeType)
    {
    case FULL:
        piped =  m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * point;
        break;
    case AXIS:
        piped =  m_cameraProjection * m_cameraTransform * point;
        break;
    case MODEL:
        piped =  m_cameraProjection * m_cameraTransform * m_objectTransform * point;
        break;
    default:
        piped =  HOMOGENEOUS_VECTOR4;
        break;
    }

    return piped;

}

void Renderer::Init()
{

}

void Renderer::DrawTriangles(const vector<vec4>& vertices, bool bDrawFaceNormals /*= false*/, const vec4* modelCentroid /*= NULL*/, float normScaleRate /*= 1*/, bool bIsCamera /*= false*/)
{
    auto it = vertices.begin();
    
    static int maxX = std::numeric_limits<int>::lowest();
    static int maxY = std::numeric_limits<int>::lowest();
    static int maxZ = std::numeric_limits<int>::lowest();

    while (it != vertices.end())
    {
        auto p1 = *(it++);
        if (it == vertices.end()) break;
        auto p2 = *(it++);
        if (it == vertices.end()) break;
        auto p3 = *(it++);

        auto nrm1 = p1;
        auto nrm2 = p2;
        auto nrm3 = p3;

        p1 = processPipeline(p1);
        p2 = processPipeline(p2);
        p3 = processPipeline(p3);

        auto viewP1 = toViewPlane(p1);
        auto viewP2 = toViewPlane(p2);
        auto viewP3 = toViewPlane(p3);

        if (m_bSolidModel)
        {
            PolygonScanConversion(viewP1, viewP2, viewP3);
        }
        else
        {
            DrawLine(viewP1, viewP2, m_wireframeColor);
            DrawLine(viewP2, viewP3, m_wireframeColor);
            DrawLine(viewP3, viewP1, m_wireframeColor);
        }
       
        if (bDrawFaceNormals)
        {
            drawFaceNormal(nrm1, nrm2, nrm3, normScaleRate);
        }

        //printf("max: %f\n%f\n%f\n%f\n%f\n%f\n", p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
    }
}

void Renderer::drawFaceNormal(const vec4& nrm1, const vec4& nrm2, const vec4& nrm3, float normScaleRate)
{
    auto nrm1_3 = Util::toCartesianForm(nrm1);
    auto nrm2_3 = Util::toCartesianForm(nrm2);
    auto nrm3_3 = Util::toCartesianForm(nrm3);

    auto subs1 = nrm3_3 - nrm1_3;
    auto subs2 = nrm2_3 - nrm1_3;

    auto faceNormal = cross(subs1, subs2);

    auto faceCenter = (nrm1_3 + nrm2_3 + nrm3_3) / 3.0f;

    auto normalizedFaceNormal = Util::isVecEqual(faceNormal, vec3(0)) ? faceNormal : normalize(faceNormal);

    normalizedFaceNormal.x *= normScaleRate;
    normalizedFaceNormal.y *= normScaleRate;
    normalizedFaceNormal.z *= normScaleRate;

    auto nP1 = processPipeline(Util::toHomogeneousForm(faceCenter));
    auto nP2 = processPipeline(Util::toHomogeneousForm(faceCenter + normalizedFaceNormal));

    DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(LIME));
}


void Renderer::PolygonScanConversion(const vec3& viewP1, const vec3& viewP2, const vec3& viewP3)
{
    ivec2 upperRight, lowerLeft;
    upperRight = { MAX3(viewP1.x, viewP2.x, viewP3.x),MAX3(viewP1.y, viewP2.y, viewP3.y) };
    lowerLeft = { MIN3(viewP1.x, viewP2.x, viewP3.x),MIN3(viewP1.y, viewP2.y, viewP3.y) };
    float maxZ = MAX3(viewP1.z, viewP2.z, viewP3.z);

    upperRight.x = upperRight.x > m_width  ? m_width  : upperRight.x;
    upperRight.y = upperRight.y > m_height ? m_height : upperRight.y;

    lowerLeft.x  = lowerLeft .x > m_width  ? m_width  : lowerLeft.x;
    lowerLeft.y  = lowerLeft .y > m_height ? m_height : lowerLeft.y;


    for (int x = lowerLeft.x ; x <= upperRight.x; x++)
    {
        for (int y = lowerLeft.y ; y <= upperRight.y; y++)

            if (isPointInTriangle({ x,y }, { viewP1.x,viewP1.y }, { viewP2.x,viewP2.y }, { viewP3.x,viewP3.y }))
            {
                putPixel(x, y, maxZ, m_polygonColor);
            }
    }

    DrawLine(viewP1, viewP2, COLOR(LIME));
    DrawLine(viewP2, viewP3, COLOR(LIME));
    DrawLine(viewP3, viewP1, COLOR(LIME));
}

void Renderer::drawVerticesNormals(const vector<vec4>& vertices, const vector<vec4>& normals, float normScaleRate)
{
    for (int i = 0; i < normals.size() && i < vertices.size(); i++)
    {
        auto vertex       = Util::toCartesianForm(vertices[i]);
        auto vertexNormal = Util::toCartesianForm(normals[i] );

        auto normalizedVertexNormal = Util::isVecEqual(vertexNormal, vec3(0)) ? vertexNormal : normalize(vertexNormal);

        normalizedVertexNormal.x *= normScaleRate;
        normalizedVertexNormal.y *= normScaleRate;
        normalizedVertexNormal.z *= normScaleRate;

        auto nP1 = processPipeline(Util::toHomogeneousForm(vertex));
        auto nP2 = processPipeline(Util::toHomogeneousForm(vertex + normalizedVertexNormal));

        DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(RED));
    }
}


void Renderer::drawBordersCube(CUBE borderCube)
{
    for (auto line : borderCube.lines)
    {
        DrawLine(toViewPlane(processPipeline(line.first)), toViewPlane(processPipeline(line.second)), COLOR(BLUE));
    }
}


// glm::vec3 Renderer::toViewPlane(const glm::vec4& point)
// {
//     // convert to raster space 
//     vec3 screenPoint;
// 
//     vec3 cartPoint = Util::toCartesianForm(point);
// 
//     screenPoint.x = cartPoint.x;
//     screenPoint.y = cartPoint.y;
//     screenPoint.z = cartPoint.z;
// 
// //     printf("x = %f, y = %f, z = %f\n", cartPoint.x, cartPoint.y, cartPoint.z);
// 
//     screenPoint.x = round((screenPoint.x - (m_width / 2.0f)) * (VIEW_SCALING * m_width) + (m_width / 2.0f));
//     screenPoint.y = round((screenPoint.y - (m_height / 2.0f)) * (VIEW_SCALING * m_height) + (m_height / 2.0f));
// 
// //     screenPoint.x = ((m_width  / 2 ) * screenPoint.x +   ( m_width  / 2));
// //     screenPoint.y = ((m_height / 2)  * screenPoint.y +    (m_height / 2));
//     screenPoint.z = ((screenPoint.z * ((m_projParams.zFar - m_projParams.zNear) / 2) + ((m_projParams.zFar + m_projParams.zNear) / 2)));
// 
//     return vec3(screenPoint.x, screenPoint.y, screenPoint.z);
// }


glm::vec3 Renderer::toViewPlane(const glm::vec4& point)
{

    // convert to raster space 
    vec3 screenPoint;

    vec3 cartPoint = Util::toCartesianForm(point);

    screenPoint.x = cartPoint.x;
    screenPoint.y = cartPoint.y;
    screenPoint.z = cartPoint.z;

    //     printf("x = %f, y = %f, z = %f\n", cartPoint.x, cartPoint.y, cartPoint.z);

    //     screenPoint.x = round((screenPoint.x - (m_width  / 2.0f)) * (VIEW_SCALING * m_width ) + (m_width  / 2.0f));
    //     screenPoint.y = round((screenPoint.y - (m_height / 2.0f)) * (VIEW_SCALING * m_height) + (m_height / 2.0f));

    screenPoint.x = round(((m_width / 2.f) * screenPoint.x/((float) m_width/m_height) + (m_width / 2.f)));
    screenPoint.y = round(((m_height / 2)  * screenPoint.y + (m_height / 2)));
//     screenPoint.z = round(((screenPoint.z * ((m_projParams.zFar - m_projParams.zNear) / 2) + ((m_projParams.zFar + m_projParams.zNear) / 2))));

    return vec3(screenPoint.x, screenPoint.y, screenPoint.z);
//     // convert to raster space 
//     vec3 screenPoint;
// 
//     screenPoint.x = ((point.x + 1) * m_width / 2.0f);
//     screenPoint.y = ((point.y + 1) * m_height / 2.0f);
//     //   screenPoint.z = ((point.z + 1) * (m_height + m_width) / 2.0f);
// 
//     screenPoint.x = round((screenPoint.x - (m_width / 2.0f)) * (250.0f / m_width) + (m_width / 2.0f));
//     screenPoint.y = round((screenPoint.y - (m_height / 2.0f)) * (250.0f / m_height) + (m_height / 2.0f));
//     screenPoint.z = ((point.z * ((m_projParams.zFar - m_projParams.zNear) / 2) + ((m_projParams.zFar + m_projParams.zNear) / 2)));
// 
// 
//     if (screenPoint.x <0 || screenPoint.x >m_width || screenPoint.y < 0 || screenPoint.y > m_height)
//     {
//         screenPoint.z = numeric_limits<float>::lowest();
//     }
//     else
//     {
//         screenPoint.z = ((point.z * ((m_projParams.zFar - m_projParams.zNear) / 2) + ((m_projParams.zFar + m_projParams.zNear) / 2)));
//     }
//     
//     return vec3((int)screenPoint.x, (int)screenPoint.y, screenPoint.z);
}

glm::vec3 Renderer::Barycentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
 
    glm::vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = glm::dot(v0, v0);
    float d01 = glm::dot(v0, v1);
    float d11 = glm::dot(v1, v1);
    float d20 = glm::dot(v2, v0);
    float d21 = glm::dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;
    
    return { u ,v ,w };
}


// bool Renderer::is_point_in_triangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
// {
// 
//     
// 
//     vec2 v0 = b - a, v1 = c - a, v2 = p - a;
// 
//     vec2 d00 = v0 * v0;
//     vec2 d01 = v0 * v1;
//     vec2 d11 = v1 * v1;
//     vec2 d20 = v2 * v0;
//     vec2 d21 = v2 * v1;
//     vec2 denom = d00 * d11 - d01 * d01;
// 
//     // compute parametric coordinates
//     float v = (d11 * d20 - d01 * d21) / denom;
//     float w = (d00 * d21 - d01 * d20) / denom;
//     return v >= 0. && w >= 0. && v + w <= 1.;
// }


BOOL Renderer::isPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
    vec3 bc = Barycentric(p, a, b, c);
    return p == a || p == b || p == c || (Util::isInRange(bc.x ,0,1) && Util::isInRange(bc.y, 0, 1) && Util::isInRange(bc.z, 0, 1));
//     return barycentricCoord.u <= 1 && barycentricCoord.y <= 1 && barycentricCoord.z <= 1 && barycentricCoord.x >= 0 && barycentricCoord.y >= 0 && barycentricCoord.z >= 0 ;
}

void Renderer::setProjectionParams(PROJ_PARAMS projParams)
{
    m_projParams = projParams;
}

PROJ_PARAMS Renderer::getProjectionParams()
{
    return m_projParams; 
}

void Renderer::SetCameraTransform(const mat4x4 & cTransform)
{
    m_cameraTransform = cTransform;
}

void Renderer::SetProjection(const mat4x4 & projection)
{
    m_cameraProjection = projection;
}

void Renderer::SetObjectMatrices(const mat4x4 & oTransform, const mat4x4 & nTransform)
{
    m_objectTransform = oTransform;
    m_normalTransform = nTransform;
}


void Renderer::putPixel(int i, int j, float d, const vec4& color)
{
    if (i < 0) return; if (i >= m_width) return;
    if (j < 0) return; if (j >= m_height) return;
    if (putZ(i, j, d))
    {
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = color.x;
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = color.y;
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = color.z;
    }
    else if(color.x == 0 && color.y == 0 && color.z ==0 && zBuffer[Z_BUF_INDEX(m_width, i, j)] == numeric_limits<float>::lowest())
    {
        printf("i = %d, j = %d, d = %f\n");
    }

}

bool Renderer::putZ(int x, int y, float d)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return false;
    }
    
    if (d < zBuffer[Z_BUF_INDEX(m_width, x, y)])
    {
//     fprintf(stderr, "FALSE - x,y:%d,%d Index is %d, zBuffer value is %f, depth value is %f\n", x, y, Z_BUF_INDEX(m_width, x, y), zBuffer[Z_BUF_INDEX(m_width, x, y)], d);
//     printf("zBuffer for (%d,%d) = %f, and depth is %f\n", x, y, zBuffer[Z_BUF_INDEX(m_width, x, y)], d);
        return false;
    }
    zBuffer[Z_BUF_INDEX(m_width, x, y)] = d;
//     fprintf(stderr, "TRUE - x,y:%d,%d Index is %d, zBuffer value is %f, depth value is %f\n", x, y, Z_BUF_INDEX(m_width, x, y), zBuffer[Z_BUF_INDEX(m_width, x, y)], d);
    return true;
}

void Renderer::DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
{

    int x0 = static_cast<int>(round(p1.x));
    int y0 = static_cast<int>(round(p1.y));
    int x1 = static_cast<int>(round(p2.x));
    int y1 = static_cast<int>(round(p2.y));


    int resSize = 1;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? resSize : -resSize;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? resSize : -resSize;
    int dm = MAX(dx, /*MAX(*/dy/*, dz)*/), i = dm;
    x1 = y1 = dm / 2;

    for (; ; )
    {
        putPixel(x0, y0, MAX(p1.z,p2.z), color); //Printing points here
        if (i <= 0) break;
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
        i  -= resSize;
    }
}

void Renderer::putPixel(int x, int y, bool steep, float d, const vec4& color)
{
    if (steep)
    {
        putPixel(y, x, d, color);
    }
    else
    {
        putPixel(x, y, d, color);
    }
}

void Renderer::createBuffers(int w, int h)
{
    createOpenGLBuffer(); //Do not remove this line.
    colorBuffer = new float[3*w*h];
    zBuffer     = new float[w*h];
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = 0.f;
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = 0.f;
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = 0.f;
            zBuffer    [Z_BUF_INDEX    (m_width, i, j)   ] = std::numeric_limits<float>::lowest();
        }
    }
}

void Renderer::setWorldTransformation(mat4x4 worldTransformation)
{
    m_worldTransformation = worldTransformation;
}

void Renderer::setSolidColor(bool bShowSolidColor)
{
    m_bSolidModel = bShowSolidColor;
}

void Renderer::orderPoints(float& x1, float& x2, float& y1, float& y2, float& d1, float& d2)
{
    if (isSlopeBiggerThanOne(x1, x2, y1, y2))
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
        std::swap(x1, d1);
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
        std::swap(d1, d2);
    }
}

void Renderer::drawAxis()
{
    vec4 axisX = m_worldTransformation[0];
    vec4 axisY = m_worldTransformation[1];
    vec4 axisZ = m_worldTransformation[2];
    axisX.w = 1;
    axisY.w = 1;
    axisZ.w = 1;

    vec4 zeroPoint = HOMOGENEOUS_VECTOR4;

    axisX     = processPipeline(axisX,     AXIS);
    axisY     = processPipeline(axisY,     AXIS);
    axisZ     = processPipeline(axisZ,     AXIS);
    zeroPoint = processPipeline(zeroPoint, AXIS);
// 
    axisX = mat4x4(SCALING_MATRIX4(2))*axisX;
    axisY = mat4x4(SCALING_MATRIX4(2))*axisY;
    axisZ = mat4x4(SCALING_MATRIX4(2))*axisZ;

    auto viewAxisX     = toViewPlane(axisX);
    auto viewAxisY     = toViewPlane(axisY);
    auto viewAxisZ     = toViewPlane(axisZ);
    auto viewZeroPoint = toViewPlane(zeroPoint);

    viewZeroPoint.z = viewAxisX.z = viewAxisY.z = viewAxisZ.z = numeric_limits<float>::lowest() + numeric_limits<float>::epsilon();


    DrawLine(viewZeroPoint, viewAxisX, COLOR(X_COL));
    DrawLine(viewZeroPoint, viewAxisY, COLOR(Y_COL));
    DrawLine(viewZeroPoint, viewAxisZ, COLOR(Z_COL));

}

void Renderer::drawModelAxis()
{
    vec4 axisX = { 1,0,0,1 };
    vec4 axisY = { 0,1,0,1 };
    vec4 axisZ = { 0,0,1,1 };
    vec4 zeroPoint = { m_objectTransform[0][3],m_objectTransform[1][3],m_objectTransform[2][3] , 1 };

    axisX     = processPipeline(axisX,     MODEL);
    axisY     = processPipeline(axisY,     MODEL);
    axisZ     = processPipeline(axisZ,     MODEL);
    zeroPoint = processPipeline(zeroPoint, MODEL);

    auto viewAxisX     = toViewPlane(axisX);
    auto viewAxisY     = toViewPlane(axisY);
    auto viewAxisZ     = toViewPlane(axisZ);
    auto viewZeroPoint = toViewPlane(zeroPoint);

    DrawLine(viewZeroPoint, viewAxisX, COLOR(X_COL));
    DrawLine(viewZeroPoint, viewAxisY, COLOR(Y_COL));
    DrawLine(viewZeroPoint, viewAxisZ, COLOR(Z_COL));

}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
    // Creates a unique identifier for an opengl texture.
    glGenTextures(1, &glScreenTex);
    // Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
    glGenVertexArrays(1, &glScreenVtc);
    GLuint buffer;
    // Makes this VAO the current one.
    glBindVertexArray(glScreenVtc);
    // Creates a unique identifier for a buffer.
    glGenBuffers(1, &buffer);
    // (-1, 1)____(1, 1)
    //	     |\  |
    //	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
    //	     |__\|
    // (-1,-1)    (1,-1)
    const GLfloat vtc[]={
        -1, -1,
         1, -1,
        -1,  1,
        -1,  1,
         1, -1,
         1,  1
    };
    const GLfloat tex[]={
        0,0,
        1,0,
        0,1,
        0,1,
        1,0,
        1,1};
    // Makes this buffer the current one.
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // This is the opengl way for doing malloc on the gpu. 
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), nullptr, GL_STATIC_DRAW);
    // memcopy vtc to buffer[0,sizeof(vtc)-1]
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
    // memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);
    // Loads and compiles a sheder.
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    // Make this program the current one.
    glUseProgram( program );
    // Tells the shader where to look for the vertex position data, and the data dimensions.
    GLint  vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,nullptr );
    // Same for texture coordinates data.
    GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

    //glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

    // Tells the shader to use GL_TEXTURE0 as the texture id.
    glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, IN float d1, IN float d2, OUT float* pDx, OUT float* pDy, OUT float* pDd)
{
    *pDx = fabs(x2 - x1);
    *pDy = fabs(y2 - y1);
    *pDd = fabs(d2 - d1);
}

void Renderer::getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, OUT float* pDx, OUT float* pDy)
{
    *pDx = fabs(x2 - x1);
    *pDy = fabs(y2 - y1);
}

void Renderer::yStepErrorUpdate(float dx, float dy, float& error, int& y, const int& ystep)
{
    error -= dy;
    if (error < 0)
    {
        y += ystep;
        error += dx;
    }
}

void Renderer::ProjectPolygon(std::vector<glm::vec3>& polygon)
{
    float zMax = MAX3(polygon[0].z, polygon[1].z, polygon[2].z);
    for (auto poly : polygon)
    {
        poly.z = zMax;
    }
}

void Renderer::Fill_A_Triangle(const std::vector<glm::vec3>& polygon)
{
    pair<vec2, vec2> firstEdge(polygon[0], (polygon[1]));
    pair<vec2, vec2> secondEdge(polygon[0], (polygon[2]));

    float firstEdgeFirstX   = firstEdge.first .x;
    float firstEdgeFirstY   = firstEdge.first .y;
    float firstEdgeSecondX  = firstEdge.second.x;
    float firstEdgeSecondY  = firstEdge.second.y;

    float secondEdgeFirstX  = secondEdge.first .x;
    float secondEdgeFirstY  = secondEdge.first .y;
    float secondEdgeSecondX = secondEdge.second.x;
    float secondEdgeSecondY = secondEdge.second.y;

    float dX_1 = 0, dY_1 = 0;
    float dX_2 = 0, dY_2 = 0;

    getDeltas(firstEdgeFirstX, firstEdgeFirstY, firstEdgeSecondX, firstEdgeSecondY, &dX_1, &dY_1);
    getDeltas(secondEdgeFirstX, secondEdgeFirstY, secondEdgeSecondX, secondEdgeSecondY, &dX_2, &dY_2);

    float firstEdgeGradient = (dY_1 != 0) ? dX_1 / dY_1 : 1.f;
    float secondEdgeGradient = (dY_2 != 0) ? dX_2 / dY_2 : -1.f;
   
    int xEnd = static_cast<int>(secondEdgeFirstX);
    int yEnd = static_cast<int>(firstEdgeSecondY);
    int x = static_cast<int>(firstEdgeFirstX);
    for (int y = static_cast<int>(firstEdgeFirstY); y <= yEnd; y++)
    {
        DrawLine(vec3(x, y, polygon[0].z), vec3(xEnd, yEnd, polygon[0].z), { 0, 0, 0, 1 });
        x -= firstEdgeGradient;
        xEnd += secondEdgeGradient;
    }
}

void Renderer::Fill_V_Triangle(const std::vector<glm::vec3>& polygon)
{
    pair<vec2, vec2> firstEdge(polygon[0], (polygon[1]));
    pair<vec2, vec2> secondEdge(polygon[0], (polygon[2]));

    float firstEdgeFirstX = firstEdge.first.x;
    float firstEdgeFirstY = firstEdge.first.y;
    float firstEdgeSecondX = firstEdge.second.x;
    float firstEdgeSecondY = firstEdge.second.y;

    float secondEdgeFirstX = secondEdge.first.x;
    float secondEdgeFirstY = secondEdge.first.y;
    float secondEdgeSecondX = secondEdge.second.x;
    float secondEdgeSecondY = secondEdge.second.y;

    float dX_1 = 0, dY_1 = 0;
    float dX_2 = 0, dY_2 = 0;

    getDeltas(firstEdgeFirstX, firstEdgeFirstY, firstEdgeSecondX, firstEdgeSecondY, &dX_1, &dY_1);
    getDeltas(secondEdgeFirstX, secondEdgeFirstY, secondEdgeSecondX, secondEdgeSecondY, &dX_2, &dY_2);

    float firstEdgeGradient = (dY_1 != 0) ? dX_1 / dY_1 : -1.f;
    float secondEdgeGradient = (dY_2 != 0) ? dX_2 / dY_2 : 1.f;

    float xEnd =                  secondEdgeFirstX;
    int   yEnd = static_cast<int>(round(firstEdgeSecondY));
    float x    =                  firstEdgeFirstX;
    for (int y = static_cast<int>(firstEdgeFirstY); y <= yEnd; y++)
    {
        DrawLine(vec3(x, y, polygon[0].z), vec3(xEnd, y, polygon[0].z), { 0, 0, 0, 1 });
        x += firstEdgeGradient;
        xEnd -= secondEdgeGradient;
    }
}

void Renderer::createOpenGLBuffer()
{
    // Makes GL_TEXTURE0 the current active texture unit
    glActiveTexture(GL_TEXTURE0);
    // Makes glScreenTex (which was allocated earlier) the current texture.
    glBindTexture(GL_TEXTURE_2D, glScreenTex);
    // malloc for a texture on the gpu.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{
    // Makes GL_TEXTURE0 the current active texture unit
    glActiveTexture(GL_TEXTURE0);
    // Makes glScreenTex (which was allocated earlier) the current texture.
    glBindTexture(GL_TEXTURE_2D, glScreenTex);
    // memcopy's colorBuffer into the gpu.
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, colorBuffer);
    // Tells opengl to use mipmapping
    glGenerateMipmap(GL_TEXTURE_2D);
    // Make glScreenVtc current VAO
    glBindVertexArray(glScreenVtc);
    // Finally renders the data.
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::ClearColorBuffer()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = m_bgColor.x;
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = m_bgColor.y;
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = m_bgColor.z;
        }
    }


}

void Renderer::Viewport(int w, int h)
{
    if (w == m_width && h == m_height)
    {
        return;
    }
    m_width = w;
    m_height = h;
    delete[] colorBuffer;
    delete[] zBuffer;
    colorBuffer = new float[3 * h*w];
    zBuffer = new float[h*w];
    createOpenGLBuffer();
}


vec4 Renderer::GetBgColor()
{
    return m_bgColor;
}

void Renderer::SetBgColor(const glm::vec4& newBgColor)
{
    m_bgColor = newBgColor;
}

vec4 Renderer::GetPolygonColor()
{
    return m_polygonColor;
}

void Renderer::SetPolygonColor(const glm::vec4& newMeshColor)
{
    m_polygonColor = newMeshColor;
}

vec4 Renderer::GetWireframeColor()
{
    return m_wireframeColor;
}

void Renderer::SetWireframeColor(const glm::vec4& newWireframeColor)
{
    m_wireframeColor = newWireframeColor;
}

void Renderer::ClearDepthBuffer()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            zBuffer[Z_BUF_INDEX(m_width, i, j)] = -std::numeric_limits<float>::infinity();
        }
    }
}