#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "Defs.h"

/*
 * Renderer class. This class takes care of all the rendering operations needed for rendering a full scene to the screen.
 * It contains all the data structures we learned in class plus your own data structures.
 */
class Renderer
{
private:
    // 3*width*height
    float *colorBuffer;
    // width*height
    float *zBuffer;

    
    // Screen dimensions
    int m_width, m_height;


    // Draws a pixel in location p with color color
    void putPixel(int i, int j, float d, const glm::vec4& color );
    void putPixel(int x, int y, bool steep, float d, const glm::vec4& color);
    bool putZ(int x, int y, float d);
    // creates float array of dimension [3,w,h]
    void createBuffers(int w, int h);
    //##############################
    //##openGL stuff. Don't touch.##
    //##############################
    GLuint glScreenTex;
    GLuint glScreenVtc;
    void createOpenGLBuffer();
    void initOpenGLRendering();
    //##############################

    glm::mat4x4 m_worldTransformation;
    glm::mat4x4 m_cameraTransform;
    glm::mat4x4 m_cameraProjection;
    glm::mat4x4 m_objectTransform;
    glm::mat4x4 m_normalTransform;

    PROJ_PARAMS m_projParams;

    glm::vec4 m_bgColor;
    glm::vec4 m_polygonColor;
    glm::vec4 m_wireframeColor;

    glm::vec3 m_minCoords;
    glm::vec3 m_maxCoords;

    glm::vec3 toViewPlane(const glm::vec4& point);

    glm::vec3 Barycentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c);
//     BOOL is_point_in_triangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
    BOOL isPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c);
    void getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, IN float d1, IN float d2, OUT float* pDx, OUT float* pDy, OUT float* pDd);
    void getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, OUT float* pDx, OUT float* pDy);
    void yStepErrorUpdate(float dx, float dy, float& error, int& y, const int& ystep);
    bool m_bSolidModel;
    void ProjectPolygon(std::vector<glm::vec3>& polygon);
    void Fill_A_Triangle(const std::vector<glm::vec3>& polygon);
    void Fill_V_Triangle(const std::vector<glm::vec3>& polygon);
public:
    Renderer();
    Renderer(int w, int h);
    ~Renderer();
    glm::vec4 processPipeline(const glm::vec4& point, PIPE_TYPE pipeType = FULL);
    // Local initializations of your implementation
    void Init();

    // Draws a line by Bresenham algorithm: 
    void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);
    void PolygonScanConversion(const glm::vec3& viewP1, const glm::vec3& viewP2, const glm::vec3& viewP3);
    void drawVerticesNormals(const std::vector<glm::vec4>& vertices, const std::vector<glm::vec4>& normals, float normScaleRate);
    // Draws wireframe triangles to the color buffer
    void DrawTriangles(const std::vector<glm::vec4>& vertices, bool bDrawFaceNormals = false, const glm::vec4* modelCentroid = nullptr, float normScaleRate = 1, bool bIsCamera = false);
    void drawFaceNormal(const glm::vec4& nrm1, const glm::vec4& nrm2, const glm::vec4& nrm3, float normScaleRate);
    // Draws surrounding border cube;
    void drawBordersCube(CUBE borderCube);
    // Sets the camera transformations with relation to world coordinates
    void SetCameraTransform(const glm::mat4x4& cTransform);

    // Sets the camera projection (perspective, orthographic etc...)
    void SetProjection(const glm::mat4x4& projection);

    // Sets the transformations for model and normals. The object transformations 
    // decide the spacial relations of the object with respect to the world.
    void SetObjectMatrices(const glm::mat4x4& oTransform, const glm::mat4x4& nTransform);

    // Swaps between the back buffer and front buffer, as explained in class.
    // https://en.wikipedia.org/wiki/Multiple_buffering#Double_buffering_in_computer_graphics
    void SwapBuffers();

    // Sets the color buffer to a new color (all pixels are set to this color).
    void ClearColorBuffer();

    // Resize the buffer.
    void Viewport(int w, int h);

    glm::vec4 GetBgColor();
    void SetBgColor(const glm::vec4& newBgColor);

    glm::vec4 GetPolygonColor();
    void SetPolygonColor(const glm::vec4& newMeshColor);

    glm::vec4 GetWireframeColor();
    void SetWireframeColor(const glm::vec4& newMeshColor);

    void setProjectionParams(PROJ_PARAMS projParams);

    PROJ_PARAMS getProjectionParams();

    // Clears the z buffer to zero.
    void ClearDepthBuffer();

    void setWorldTransformation(glm::mat4x4 m_worldTransformation);
    void setSolidColor(bool m_bShowSolidColor);
private:
    void orderPoints(float& x1, float& x2, float& y1, float& y2, float& d1, float& d2);
    bool isSlopeBiggerThanOne(float x1, float x2, float y1, float y2) { return (std::fabs(y2 - y1) > std::fabs(x2 - x1)); }
public:
    void drawAxis();
    void drawModelAxis();
};
