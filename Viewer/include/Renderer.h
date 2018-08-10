#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "Defs.h"

using namespace std;

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
    void putPixel(int i, int j, const glm::vec3& color );
    void putPixel(int x, int y, bool steep, const glm::vec3& color);
    void putZ(int x, int y, float d);
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

    glm::vec3 m_bgColor;
    glm::vec3 m_polygonColor;

    glm::uvec2 toViewPlane(const glm::vec3& point);

    void getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, OUT float* pDx, OUT float* pDy);
    void yStepErrorUpdate(float dx, float dy, float& error, int& y, const int& ystep);
public:
    Renderer();
    Renderer(int w, int h);
    ~Renderer();
    // Local initializations of your implementation
    void Init();

    // Draws a line by Bresenham algorithm: 
    void DrawLine(const glm::uvec2& p1, const glm::uvec2& p2, const glm::vec3& color);

    void drawVerticesNormals(const vector<glm::vec3>& vertices, const vector<glm::vec3>& normals, float normScaleRate);
    // Draws wireframe triangles to the color buffer
    void DrawTriangles(const vector<glm::vec3>* vertices, bool bDrawFaceNormals = false, const glm::vec3* modelCentroid = NULL, float normScaleRate = 1, bool bIsCamera = false);
    // Draws surrounding border cube;
    void drawBordersCube(CUBE_LINES borderCube);
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

    glm::vec3 GetBgColor();

    void SetBgColor(glm::vec3 newBgColor);

    glm::vec3 GetPolygonColor();

    void SetPolygonColor(glm::vec3 newMeshColor);

    // Clears the z buffer to zero.
    void ClearDepthBuffer();

    // Draw wide vertical and horizontal lines on the screen
    void SetDemoBuffer();

    void setWorldTransformation(glm::mat4x4 m_worldTransformation);
private:
    void orderPoints(float& x1, float& x2, float& y1, float& y2);
    bool isSlopeBiggerThanOne(float x1, float x2, float y1, float y2) { return (fabs(y2 - y1) > fabs(x2 - x1)); }
public:
    void drawAxis();
    void drawModelAxis();
};
