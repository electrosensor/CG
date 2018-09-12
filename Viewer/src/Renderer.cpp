#include <algorithm>
#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include "Util.h"
#include "Defs.h"
#include "glm/common.hpp"
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

Renderer::Renderer() : m_width(DEFAULT_WIDTH), m_height(DEFAULT_HEIGHT)
{

    initOpenGLRendering();
    createBuffers(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Renderer::Renderer(int w, int h) : m_width(w), m_height(h), m_normalTransform(I_MATRIX), m_cameraTransform(I_MATRIX), m_objectTransform(I_MATRIX), m_cameraProjection(I_MATRIX), m_worldTransformation(I_MATRIX), m_bgColor(Util::getColor(CLEAR)), m_polygonColor(Util::getColor(BLACK)), m_wireframeColor(Util::getColor(WHITE)), m_ePostEffect(NONE), m_bloomIntensity(1.f), m_bloomThreshold(1.f)
{
    initOpenGLRendering();
    createBuffers(w, h);
}

Renderer::~Renderer()
{
    delete[] colorBuffer;
    delete[] bloomBuffer;
    delete[] bloomDestBuff;
    delete[] blurredBuffer;
    delete[] zBuffer;
}

vec3 Renderer::processPipeline(vec3 point, PIPE_TYPE pipeType /*= FULL*/, glm::mat4x4* lightTransform /*= nullptr*/)
{
    glm::vec4 piped;
    glm::vec4 homogPoint = Util::toHomogeneousForm(point);

    switch (pipeType)
    {
    case FULL:
        piped = m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * homogPoint;
        break;
    case AXIS:
        piped = m_cameraProjection * m_cameraTransform * homogPoint;
        break;
    case MODEL:
        piped = m_objectTransform * m_worldTransformation * homogPoint;
        break;
    case LIGHT:
        piped = lightTransform ? *lightTransform * m_worldTransformation * homogPoint : homogPoint;
        break;
    default:
        piped =  HOMOGENEOUS_VECTOR4;
        break;
    }

    return Util::toCartesianForm(piped);

}

Face Renderer::processPipeline(Face polygon, PIPE_TYPE pipeType /*= FULL*/, glm::mat4x4* lightTransform/* = nullptr*/)
{
    polygon.m_p1 = processPipeline(polygon.m_p1, pipeType, lightTransform);
    polygon.m_p2 = processPipeline(polygon.m_p2, pipeType, lightTransform);
    polygon.m_p3 = processPipeline(polygon.m_p3, pipeType, lightTransform);
    return polygon;
}

void Renderer::Init()
{

}

void Renderer::DrawTriangles(const std::vector<Face>& vertices, const glm::vec3* modelCentroid /*= nullptr*/, const glm::vec3 eye /*= ZERO_VEC3*/)
{
    
    for (auto it = vertices.begin(); it != vertices.end(); it++)
    {
        auto polygon = *it;
        auto pipedPolygon(processPipeline(polygon, FULL));
        auto viewPolygon(toViewPlane(pipedPolygon));
     
        DrawFaceNormal(viewPolygon);
        CalculateLights(polygon, viewPolygon, eye);

        if (m_shadingType != ST_NO_SHADING)
        {
            PolygonScanConversion(viewPolygon);
        }
        if (m_bDrawWireframe)
        {
            DrawPolygonLines(viewPolygon);
        }

    }
}


void Renderer::CalculateLights(Face &polygon, Face &viewPolygon, const glm::vec3 eye)
{
    vec3 normAndPipedNormalP1;
    vec3 normAndPipedNormalP2;
    vec3 normAndPipedNormalP3;

    if (m_shadingType == ST_FLAT)
    {
        normAndPipedNormalP1 = normalize(processPipeline(polygon.m_normal, MODEL));
        normAndPipedNormalP2 = normalize(processPipeline(polygon.m_normal, MODEL));
        normAndPipedNormalP3 = normalize(processPipeline(polygon.m_normal, MODEL));
    }

    if (m_shadingType == ST_GOURAUD)
    {
        normAndPipedNormalP1 = normalize(processPipeline(polygon.m_p1 + polygon.m_normal, MODEL));
        normAndPipedNormalP2 = normalize(processPipeline(polygon.m_p2 + polygon.m_normal, MODEL));
        normAndPipedNormalP3 = normalize(processPipeline(polygon.m_p3 + polygon.m_normal, MODEL));
    }
    if (m_shadingType == ST_PHONG)
    {
        normAndPipedNormalP1 = normalize(processPipeline(polygon.m_p1 + polygon.m_vn1, MODEL));
        normAndPipedNormalP2 = normalize(processPipeline(polygon.m_p2 + polygon.m_vn2, MODEL));
        normAndPipedNormalP3 = normalize(processPipeline(polygon.m_p3 + polygon.m_vn3, MODEL));
    }

    vec3 PipedFaceP1 = processPipeline(polygon.m_p1, MODEL);
    vec3 PipedFaceP2 = processPipeline(polygon.m_p2, MODEL);
    vec3 PipedFaceP3 = processPipeline(polygon.m_p3, MODEL);

    for (auto lightData : viewPolygon.m_diffusiveColorAndSource)
    {
        auto PipedlightCoord = processPipeline(lightData.second.first, LIGHT, &lightData.second.second);

        auto lightCoord1 = normalize(PipedFaceP1 + PipedlightCoord);
        auto lightCoord2 = normalize(PipedFaceP2 + PipedlightCoord);
        auto lightCoord3 = normalize(PipedFaceP3 + PipedlightCoord);

        auto diffusiveProductP1 = lightData.first.first * lightData.first.second * dot(normAndPipedNormalP1, lightCoord1);
        auto diffusiveProductP2 = lightData.first.first * lightData.first.second * dot(normAndPipedNormalP2, lightCoord2);
        auto diffusiveProductP3 = lightData.first.first * lightData.first.second * dot(normAndPipedNormalP3, lightCoord3);

        viewPolygon.m_actualColorP1 += diffusiveProductP1;
        viewPolygon.m_actualColorP2 += diffusiveProductP2;
        viewPolygon.m_actualColorP3 += diffusiveProductP3;
    }

    for (auto lightData : viewPolygon.m_speculativeColorAndSource)
    {
        auto PipedlightCoord = processPipeline(lightData.second.first, LIGHT, &lightData.second.second);

        auto lightCoord1 = normalize(PipedFaceP1 + PipedlightCoord);
        auto lightCoord2 = normalize(PipedFaceP2 + PipedlightCoord);
        auto lightCoord3 = normalize(PipedFaceP3 + PipedlightCoord);

        glm::vec3 reflection1 = normalize(PipedFaceP1 + (2.f * dot(normAndPipedNormalP1, lightCoord1)) * normAndPipedNormalP1 - lightCoord1);
        glm::vec3 reflection2 = normalize(PipedFaceP2 + (2.f * dot(normAndPipedNormalP2, lightCoord2)) * normAndPipedNormalP2 - lightCoord2);
        glm::vec3 reflection3 = normalize(PipedFaceP3 + (2.f * dot(normAndPipedNormalP3, lightCoord3)) * normAndPipedNormalP3 - lightCoord3);

        glm::vec3 pipedEye = processPipeline(eye, LIGHT, &(inverse(m_cameraTransform)));

        glm::vec3 curr_eye1 = normalize(PipedFaceP1 + pipedEye);
        glm::vec3 curr_eye2 = normalize(PipedFaceP2 + pipedEye);
        glm::vec3 curr_eye3 = normalize(PipedFaceP3 + pipedEye);

        glm::vec4 specLight1 = lightData.first.first * lightData.first.second * glm::pow(dot(reflection1, curr_eye1), viewPolygon.m_surface->m_shininess);
        glm::vec4 specLight2 = lightData.first.first * lightData.first.second * glm::pow(dot(reflection2, curr_eye2), viewPolygon.m_surface->m_shininess);
        glm::vec4 specLight3 = lightData.first.first * lightData.first.second * glm::pow(dot(reflection3, curr_eye3), viewPolygon.m_surface->m_shininess);

        viewPolygon.m_actualColorP1 += specLight1;
        viewPolygon.m_actualColorP2 += specLight2;
        viewPolygon.m_actualColorP3 += specLight3;
    }
}

void Renderer::DrawPolygonLines(const Face& polygon)
{
    DrawLine(polygon.m_p1, polygon.m_p2, m_wireframeColor);
    DrawLine(polygon.m_p2, polygon.m_p3, m_wireframeColor);
    DrawLine(polygon.m_p3, polygon.m_p1, m_wireframeColor);
}

void Renderer::DrawFaceNormal(Face& face)
{
    auto faceCenter = face.m_faceCenter;
    auto normalizedFaceNormal = face.m_normal;

    normalizedFaceNormal.x *= m_faceNormScaleFactor;
    normalizedFaceNormal.y *= m_faceNormScaleFactor;
    normalizedFaceNormal.z *= m_faceNormScaleFactor;

    auto nP1 = processPipeline(faceCenter);
    auto nP2 = processPipeline(faceCenter + normalizedFaceNormal);
    if (m_bDrawFaceNormals)
    {
        DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(LIME));
    }
}


void Renderer::PolygonScanConversion(Face& polygon)
{

    ivec2 upperRight, lowerLeft;
    upperRight = { MAX3(polygon.m_p1.x, polygon.m_p2.x, polygon.m_p3.x), MAX3(polygon.m_p1.y, polygon.m_p2.y, polygon.m_p3.y) };
    lowerLeft  = { MIN3(polygon.m_p1.x, polygon.m_p2.x, polygon.m_p3.x), MIN3(polygon.m_p1.y, polygon.m_p2.y, polygon.m_p3.y) };
    float maxZ = MAX3(polygon.m_p1.z, polygon.m_p2.z, polygon.m_p3.z);

    upperRight.x = upperRight.x > m_width  ? m_width  : upperRight.x;
    upperRight.y = upperRight.y > m_height ? m_height : upperRight.y;

    lowerLeft.x  = lowerLeft .x > m_width  ? m_width  : lowerLeft.x;
    lowerLeft.y  = lowerLeft .y > m_height ? m_height : lowerLeft.y;


    for (int x = lowerLeft.x; x <= upperRight.x; x++)
    {
        for (int y = lowerLeft.y; y <= upperRight.y; y++)
        {

            if (isPointInTriangle({ x,y }, { polygon.m_p1.x,polygon.m_p1.y }, { polygon.m_p2.x,polygon.m_p2.y }, { polygon.m_p3.x,polygon.m_p3.y }))
            {
                vec3 baryVec = { 1.f, 1.f, 1.f };
                if (m_shadingType != ST_SOLID) {
                    baryVec = Barycentric({ x,y }, polygon.m_p1, polygon.m_p2, polygon.m_p3);
                }
                vec4 actualColor = ZERO_VEC4;
//                 static int first = 1 + (rand() % 20) + (((int)(polygon.m_p1.x * 1000)) % 200);
//                 static int second = 1 + (rand() % 20) + (((int)(polygon.m_p1.y * 1000)) % 200);
//                 static int third = 1 + (rand() % 20) + (((int)(polygon.m_p1.z * 1000)) % 200); 
                switch (m_generatedTexture)
                {
                    case GT_CRYSTAL:
                    {
                        actualColor = (
                            ((pow(baryVec.x, sin(((int)(polygon.m_faceCenter.x * 1000) % 200) + baryVec.x)) / 3) * (polygon.m_actualColorP1)) +
                            ((pow(baryVec.y, sin(((int)(polygon.m_faceCenter.y * 1000) % 200) + baryVec.y)) / 3) * (polygon.m_actualColorP2)) +
                            ((pow(baryVec.z, sin(((int)(polygon.m_faceCenter.z * 1000) % 200) + baryVec.z)) / 3) * (polygon.m_actualColorP3))
                            );

                    } break;
                    case GT_RUG:
                    {
                        actualColor = (
                            ((pow(1.3f + (cos(((int)(polygon.m_faceCenter.x * 1000) % 16) * baryVec.x)), sin(((int)(polygon.m_faceCenter.x * 1000) % 16) * baryVec.x)) / 3) * (polygon.m_actualColorP1)) +
                            ((pow(1.3f + (cos(((int)(polygon.m_faceCenter.y * 1000) % 16) * baryVec.y)), sin(((int)(polygon.m_faceCenter.y * 1000) % 16) * baryVec.y)) / 3) * (polygon.m_actualColorP2)) +
                            ((pow(1.3f + (cos(((int)(polygon.m_faceCenter.z * 1000) % 16) * baryVec.z)), sin(((int)(polygon.m_faceCenter.z * 1000) % 16) * baryVec.z)) / 3) * (polygon.m_actualColorP3))
                            );
                    } break;
                    default:
                    {
                        actualColor = (
                            ((baryVec.x / 3) * (polygon.m_actualColorP1)) +
                            ((baryVec.y / 3) * (polygon.m_actualColorP2)) +
                            ((baryVec.z / 3) * (polygon.m_actualColorP3))
                            );
                    } break;
                }

                putPixel(x, y, maxZ, actualColor, &polygon);
            }
        }
    }

}

void Renderer::drawVerticesNormals(const vector<vec3>& vertices, const vector<vec3>& normals, float normScaleRate)
{
    for (int i = 0; i < normals.size() && i < vertices.size(); i++)
    {
        auto vertex       = vertices[i];
        auto vertexNormal = normals[i];

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


glm::vec3 Renderer::toViewPlane(const glm::vec3& point)
{
    // convert to raster space 
    vec3 screenPoint;

    vec3 cartPoint = point;

    screenPoint.x = cartPoint.x;
    screenPoint.y = cartPoint.y;
    screenPoint.z = cartPoint.z;

    screenPoint.x = round(((m_width / 2.f) * screenPoint.x/((float) m_width/m_height) + (m_width / 2.f)));
    screenPoint.y = round(((m_height / 2)  * screenPoint.y + (m_height / 2)));
//     screenPoint.z = round(((screenPoint.z * ((m_projParams.zFar - m_projParams.zNear) / 2) + ((m_projParams.zFar + m_projParams.zNear) / 2))));

    return vec3(screenPoint.x, screenPoint.y, screenPoint.z);

}

Face Renderer::toViewPlane(Face polygon)
{
    polygon.m_p1 = toViewPlane(polygon.m_p1);
    polygon.m_p2 = toViewPlane(polygon.m_p2);
    polygon.m_p3 = toViewPlane(polygon.m_p3);

    return polygon;
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


void Renderer::putPixel(int i, int j, float d, const vec4& color, Face* face)
{
    if (i < 0) return; if (i >= m_width) return;
    if (j < 0) return; if (j >= m_height) return;
    if (putZ(i, j, d))
    {
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = color.x;
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = color.y;
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = color.z;
        
        if (face)
        {
            float brightness = glm::dot({ color.x,color.y,color.z }, vec3(m_bloomThreshold.x, m_bloomThreshold.y, m_bloomThreshold.z));

//             float dotIntensity = sqrt(color.x*color.x / 3 + color.y*color.y / 3 + color.z*color.z / 3);
            if (brightness > m_bloomThresh)
            {
                vec3 color3Vec = { color.x,color.y,color.z };
                bloomBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = color.x;
                bloomBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = color.y;
                bloomBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = color.z;
            }
        }
        }

    else if(color.x == 0 && color.y == 0 && color.z == 0 && zBuffer[Z_BUF_INDEX(m_width, i, j)] == numeric_limits<float>::lowest())
    {
//        printf("i = %d, j = %d, d = %f\n");
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
    colorBuffer   = new float[3 * w * h];
    blurredBuffer = new float[3 * w * h];
    bloomBuffer   = new float[3 * w * h];
    bloomDestBuff = new float[3 * w * h];
    zBuffer       = new float[1 * w * h];

    memset(colorBuffer  ,                  0.f                   , sizeof(float) * 3 * w * h);
    memset(blurredBuffer,                  0.f                   , sizeof(float) * 3 * w * h);
    memset(bloomBuffer  ,                  0.f                   , sizeof(float) * 3 * w * h);
    memset(bloomDestBuff,                  0.f                   , sizeof(float) * 3 * w * h);
    memset(zBuffer      , -std::numeric_limits<float>::infinity(), sizeof(float) * 1 * w * h);

}

void Renderer::SetWorldTransformation(mat4x4 worldTransformation)
{
    m_worldTransformation = worldTransformation;
}

// void Renderer::SetSolidColor(bool bShowSolidColor)
// {
//     m_bSolidModel = bShowSolidColor;
// }

void Renderer::SetShadingType(SHADING_TYPE shading)
{
    m_shadingType = shading;
}

void Renderer::DrawWireframe(bool bDrawn)
{
    m_bDrawWireframe = bDrawn;
}

void Renderer::DrawFaceNormal(bool bDrawn)
{
    m_bDrawFaceNormals = bDrawn;
}

void Renderer::SetFaceNormScaleFactor(float scaleFactor)
{
    m_faceNormScaleFactor = scaleFactor;
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

void Renderer::DrawAxis()
{
    vec3 axisX = m_worldTransformation[0];
    vec3 axisY = m_worldTransformation[1];
    vec3 axisZ = m_worldTransformation[2];

    vec3 zeroPoint = ZERO_VEC3;

    axisX     = processPipeline(axisX,     AXIS);
    axisY     = processPipeline(axisY,     AXIS);
    axisZ     = processPipeline(axisZ,     AXIS);
    zeroPoint = processPipeline(zeroPoint, AXIS);
// 
    axisX = 2.f * axisX;
    axisY = 2.f * axisY;
    axisZ = 2.f * axisZ;

    auto viewAxisX     = toViewPlane(axisX);
    auto viewAxisY     = toViewPlane(axisY);
    auto viewAxisZ     = toViewPlane(axisZ);
    auto viewZeroPoint = toViewPlane(zeroPoint);

    viewZeroPoint.z = viewAxisX.z = viewAxisY.z = viewAxisZ.z = numeric_limits<float>::lowest() + numeric_limits<float>::epsilon();


    DrawLine(viewZeroPoint, viewAxisX, COLOR(X_COL));
    DrawLine(viewZeroPoint, viewAxisY, COLOR(Y_COL));
    DrawLine(viewZeroPoint, viewAxisZ, COLOR(Z_COL));

}

void Renderer::DrawModelAxis()
{
    vec3 axisX = { 1,0,0 };
    vec3 axisY = { 0,1,0 };
    vec3 axisZ = { 0,0,1 };
    vec3 zeroPoint = { m_objectTransform[0][3],m_objectTransform[1][3],m_objectTransform[2][3] };

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

void Renderer::SetGeneratedTexture(GENERATED_TEXTURE texture)
{
    m_generatedTexture = texture;
}

GENERATED_TEXTURE Renderer::GetGeneratedTexture()
{
    return m_generatedTexture;
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
    // Makes this VAO the current one.
    glBindVertexArray(glScreenVtc);
    // Makes GL_TEXTURE0 the current active texture unit
    glActiveTexture(GL_TEXTURE0);
    // Makes glScreenTex (which was allocated earlier) the current texture.
    glBindTexture(GL_TEXTURE_2D, glScreenTex);
    // malloc for a texture on the gpu.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glViewport(0, 0, m_width, m_height);
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

void Renderer::ProjectPolygon(Face& polygon)
{
    polygon.m_p1.z = MAX3(polygon.m_p1.z, polygon.m_p2.z, polygon.m_p3.z);
    polygon.m_p2.z = MAX3(polygon.m_p1.z, polygon.m_p2.z, polygon.m_p3.z);
    polygon.m_p3.z = MAX3(polygon.m_p1.z, polygon.m_p2.z, polygon.m_p3.z);

}

void Renderer::createOpenGLBuffer()
{



}

void Renderer::SwapBuffers()
{    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, glScreenVtc);

    // This is the opengl way for doing malloc on the gpu. 
    glBufferData(GL_ARRAY_BUFFER, mVerticesPositionsSize*sizeof(GLfloat), mVerticesPositions, GL_STATIC_DRAW);

    // memcopy vtc to buffer[0,sizeof(vtc)-1]
//     glBufferSubData(GL_ARRAY_BUFFER, 0, mVerticesPositionsSize * sizeof(GLfloat), mVerticesPositions);
// //     // memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
//     glBufferSubData(GL_ARRAY_BUFFER, mVerticesPositionsSize * sizeof(GLfloat), mVerticesColorsSize * sizeof(GLfloat), mVerticesColors);
    // Loads and compiles a sheder.

    GLuint program = InitShader("vshader.glsl", "fshader.glsl");
    // Make this program the current one.
    glUseProgram(program);
    // Tells the shader where to look for the vertex position data, and the data dimensions.
    GLint  vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


    //     // Makes GL_TEXTURE0 the current active texture unit
    glActiveTexture(GL_TEXTURE0);
    //     // Makes glScreenTex (which was allocated earlier) the current texture.
    glBindTexture(GL_TEXTURE_2D, glScreenTex);
    //     // memcopy's colorBuffer into the gpu.
    //glfwLoadTexture2D(imagepath, 0);
    glBufferData(GL_TEXTURE_2D, mVerticesColorsSize * sizeof(GLfloat), mVerticesColors, GL_STATIC_DRAW);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, mVerticesColors/*mvTexturesBuffer*/);
    //     // Tells opengl to use mipmapping
    glGenerateMipmap(GL_TEXTURE_2D);

    // Same for texture coordinates data.
    GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    //glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

    // Tells the shader to use GL_TEXTURE0 as the texture id.

    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.1f, 100.0f);

    //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    glm::mat4 View = glm::lookAt(
        glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glm::mat4 mvp = Projection * View/* * m_objectTransform*/;

    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);


//     pDispBuffer = colorBuffer;
//     
//     if (m_blurX > 1 || m_blurY > 1)
//     {
//         switch (m_ePostEffect)
//         {
//         case BLUR_SCENE:
//             pDispBuffer = blurredBuffer;
//             break;
//         case BLOOM:
//             pDispBuffer = blurredBuffer;
//             break;
//         default:
//             pDispBuffer = colorBuffer;
//             break;
//         }
//     }


// 
//     // Finally renders the data.
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    glDrawArrays(GL_TRIANGLES, 0, mVerticesPositionsSize);
}

void Renderer::ClearColorBuffer()
{
  //  glClear(GL_COLOR_BUFFER_BIT);
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = m_bgColor.x;
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = m_bgColor.y;
            colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = m_bgColor.z;
        }
    }

    if (m_ePostEffect == BLUR_SCENE)
    {
        memset(blurredBuffer, 0.f, sizeof(float) * m_width * m_height * 3);

    }
    else if (m_ePostEffect == BLOOM)
    {
        memset(bloomDestBuff, 0.f, sizeof(float) * m_width * m_height * 3);
        memset(bloomBuffer  , 0.f, sizeof(float) * m_width * m_height * 3);
    }

}

void Renderer::Viewport(int w, int h)
{
    if (w == m_width && h == m_height)
    {
        return;
    }
    m_width       = w;
    m_height      = h;
    delete[] colorBuffer;
    delete[] blurredBuffer;
    delete[] bloomDestBuff;
    delete[] zBuffer;
    colorBuffer   = new float[3 * h*w];
    blurredBuffer = new float[3 * h*w];
    bloomBuffer   = new float[3 * h*w];
    bloomDestBuff = new float[3 * h*w];
    zBuffer       = new float[h*w];
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
   // glClear(GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            zBuffer[Z_BUF_INDEX(m_width, i, j)] = -std::numeric_limits<float>::infinity();
        }
    }
}


void Renderer::applyPostEffect(int kernelSizeX, int kernelSizeY, float sigma, POST_EFFECT postEffect /*= NONE*/)
{
    static int kerSizeX = -1;
    static int kerSizeY = -1;

    float* source;
    float* destination;

    static float kernel[29][29] = { 0 };

    if (kernelSizeX <= 2 && kernelSizeY <= 2 || postEffect == NONE)
    {
        pDispBuffer = colorBuffer;
        return;
    }


    if (kerSizeX != kernelSizeX || kerSizeY != kernelSizeY || sigma != m_sigma)
    {
        kerSizeX = kernelSizeX;
        kerSizeY = kernelSizeY;
        m_sigma = sigma;

        Renderer::makeKernel(kernel, kernelSizeX, kernelSizeY, sigma);

        static int counter = 0;
        printf("%d: kernel size is x,y:%d,%d with sigma %f\n", ++counter, kernelSizeX, kernelSizeY, sigma);
        for (int kX = 0; kX < kernelSizeX; ++kX)
        {
            for (int kY = 0; kY < kernelSizeY; ++kY)
            {
                printf("%f\t\t", kernel[kX][kY]);
            }
            printf("\n\n");
        }
    }

    switch (postEffect)
    {
    case NONE:
        return;
        break;
    case BLUR_SCENE:
        source = colorBuffer;
        destination = blurredBuffer;
        break;
    case BLOOM:
        source = bloomBuffer;
        destination = bloomDestBuff;
        break;
    default:
        return;
        break;
    }

    int halfX = kernelSizeX / 2;
    int halfY = kernelSizeY / 2;

    for (int x = halfX; x < m_width - halfX; x++)
    {
        for (int y = halfY; y < m_height - halfY; y++)
        {
            for (int color = 0; color < 3; color++)
            {
                float sum = 0.0f;
                float sumBloom = 0.0f;
                for (int kX = -halfX; kX <= halfX; kX++)
                {
                    for (int kY = -halfY; kY <= halfY; kY++)
                    {
                        sum += kernel[kX + halfX][kY + halfY] * colorTruncate(source[COLOR_BUF_INDEX(m_width, x + kX, y + kY, color)]);
                        if (postEffect == BLOOM)
                        {
                            sumBloom += kernel[kX + halfX][kY + halfY] * colorTruncate(colorBuffer[COLOR_BUF_INDEX(m_width, x + kX, y + kY, color)]);
                        }
                    }
                }
                destination[COLOR_BUF_INDEX(m_width, x, y, color)] = sum;
                if (postEffect == BLOOM)
                {
                    blurredBuffer[COLOR_BUF_INDEX(m_width, x, y, color)] = sumBloom;
                }
            }
        }
    }


    if (postEffect == BLOOM)
    {   
        for (int x = 0; x < m_width; x++)
        {
            for (int y = 0; y < m_height; y++)
            {
                for (int color = 0; color < 3; color++)
                    blurredBuffer[COLOR_BUF_INDEX(m_width, x, y, color)] += m_bloomIntensity * bloomDestBuff[COLOR_BUF_INDEX(m_width, x, y, color)];
            }
        }
    }
}

float Renderer::colorTruncate(float color)
{
    if (color >= 0 && color <= 1)
    {
        return color;
    }
    else if (color < 0)
    {
        return 0;
    }
    else
    {
        return 1; 
    }
}

void Renderer::configPostEffect(POST_EFFECT postEffect, int blurX, int blurY, float sigma, float bloomIntensity, glm::vec4 bloomThreshold, float bloomThresh)
{
    m_blurX = blurX;
    m_blurY = blurY;
    m_ePostEffect = postEffect;
    m_bloomIntensity = bloomIntensity;
    m_bloomThreshold = bloomThreshold;
    m_bloomThresh = bloomThresh;
}

void Renderer::makeKernel(float gaussianKernel[][29], int kernelSizeX, int kernelSizeY, float sigma)
{
    float meanX = kernelSizeX / 2;
    float meanY = kernelSizeY / 2;
    float sum = 0.0; // For accumulating the kernel values

    for (int x = 0; x < kernelSizeX; ++x)
    {
        for (int y = 0; y < kernelSizeY; ++y)
        {
            gaussianKernel[x][y] = exp(-0.5 * (pow((x - meanX) / sigma, 2.0) + pow((y - meanY) / sigma, 2.0))) / (2 * PI * pow(sigma, 2));
            sum += gaussianKernel[x][y];
        }
    }

    // Normalize the kernel
    for (int x = 0; x < kernelSizeX ; x++)
    {
        for (int y = 0; y < kernelSizeY; y++)
        {
            gaussianKernel[x][y] /= sum;
        }
    }
}