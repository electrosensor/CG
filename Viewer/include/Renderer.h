#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

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
    int width, height;

    // Draws a pixel in location p with color color
    void putPixel(int i, int j, const glm::vec3& color );
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

    glm::mat4x4 m_cameraTransform;
    glm::mat4x4 m_cameraProjection;
    glm::mat4x4 m_cameraFrustum;
    glm::mat4x4 m_objectTransform;
    glm::mat4x4 m_normalTransform;
    glm::mat4x4 m_viewPort;

    int m_currentWidth;
    int m_currentHeight;

public:
    Renderer();
    Renderer(int w, int h);
    ~Renderer();
    // Local initializations of your implementation
    void Init();

    // Draws a line by Bresenham algorithm: 
    void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec3& color);

    void drawVerticesNormals(vector<glm::vec3> vertices, vector<glm::vec3> normals);
    // Draws wireframe triangles to the color buffer
    void DrawTriangles(const vector<glm::vec3>* vertices, bool bDrawFaceNormals = false, UINT32 normScaleRate = 1, const vector<glm::vec3>* normals=NULL);

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
    void ClearColorBuffer(const glm::vec3& color);

    // Resize the buffer.
    void Viewport(int w, int h);

    // Clears the z buffer to zero.
    void ClearDepthBuffer();

    // Draw wide vertical and horizontal lines on the screen
    void SetDemoBuffer();

    void setCurrentDims(int currentHeight, int currentWidth);
};
