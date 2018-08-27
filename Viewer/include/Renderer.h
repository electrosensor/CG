#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include "Face.h"

/*
 * Renderer class. This class takes care of all the rendering operations needed for rendering a full scene to the screen.
 * It contains all the data structures we learned in class plus your own data structures.
 */
class Renderer
{
private:
    // 3*width*height
    float *colorBuffer;
    // 3*width*height
    float *blurredBuffer;
    // 3*width*height
    float *bloomBuffer;
    // 3*width*height
    float *bloomDestBuff;
    // width*height
    float *zBuffer;

    
    // Screen dimensions
    int m_width, m_height;


    // Draws a pixel in location p with color color
    void putPixel(int i, int j, float d, const glm::vec4& color, Face* face = nullptr );
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

    glm::mat4x4       m_worldTransformation;
    glm::mat4x4       m_cameraTransform;
    glm::mat4x4       m_cameraProjection;
    glm::mat4x4       m_objectTransform;
    glm::mat4x4       m_normalTransform;

    PROJ_PARAMS       m_projParams;

    bool              m_bDrawWireframe;
    glm::vec4         m_bgColor;
    glm::vec4         m_polygonColor;
    glm::vec4         m_wireframeColor;

    glm::vec3         m_minCoords;
    glm::vec3         m_maxCoords;

    SHADING_TYPE      m_shadingType;
    GENERATED_TEXTURE m_generatedTexture;

    glm::vec3 toViewPlane(const glm::vec3& point);
    Face      toViewPlane(Face polygon);
    glm::vec3 Barycentric(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c);
    BOOL      isPointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c);
    void      getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, IN float d1, IN float d2, OUT float* pDx, OUT float* pDy, OUT float* pDd);
    void      getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, OUT float* pDx, OUT float* pDy);
    void      yStepErrorUpdate(float dx, float dy, float& error, int& y, const int& ystep);
    void      ProjectPolygon(Face& polygon);
    void      orderPoints(float& x1, float& x2, float& y1, float& y2, float& d1, float& d2);
    bool      isSlopeBiggerThanOne(float x1, float x2, float y1, float y2) { return (std::fabs(y2 - y1) > std::fabs(x2 - x1)); }

    float       m_faceNormScaleFactor;
    bool        m_bDrawFaceNormals;
    float       m_sigma;
    bool        m_bBloomActive;
    POST_EFFECT m_ePostEffect;
    float       m_bloomIntensity;
    float       * pDispBuffer;

    int         m_blurX;
    int         m_blurY;
    glm::vec4       m_bloomThreshold;
    float m_bloomThresh;
public:
    Renderer();
    Renderer(int w, int h);
    ~Renderer();
    inline glm::vec3 processPipeline(glm::vec3 point, PIPE_TYPE pipeType = FULL, glm::mat4x4* lightTransform = nullptr);
    inline Face processPipeline(Face polygon, PIPE_TYPE pipeType = FULL, glm::mat4x4* lightTransform = nullptr);
    // Local initializations of your implementation
    void Init();

    // Draws a line by Bresenham algorithm: 
    void DrawLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);
    void PolygonScanConversion(Face& polygon);
    void drawVerticesNormals(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals, float normScaleRate);
    // Draws wireframe triangles to the color buffer
    void DrawTriangles(const std::vector<Face>& vertices, const glm::vec3* modelCentroid = nullptr, const glm::vec3 eye = ZERO_VEC3);

    void CalculateLights(Face &polygon, Face &viewPolygon, const glm::vec3 eye);

    void DrawPolygonLines(const Face& polygon);

    void DrawFaceNormal(Face& face);
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

    int getHeight() { return m_height; }
    int getWidth() { return m_width; }

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

    void SetWorldTransformation(glm::mat4x4 m_worldTransformation);
    void SetShadingType(SHADING_TYPE shading);
    void DrawWireframe(bool bDrawn);

    void applyPostEffect(int kernelSizeX, int kernelSizeY, float sigma, POST_EFFECT postEffect = NONE);
    float colorTruncate(float color);
    void makeKernel(float gaussianKernel[][29], int kernelSizeX, int kernelSizeY, float sigma);
    void configPostEffect(POST_EFFECT postEffect, int blurX, int blurY, float sigma, float bloomIntensity, glm::vec4 bloomThreshold, float bloomThresh);
    void DrawFaceNormal(bool bDrawn);
    void SetFaceNormScaleFactor(float scaleFactor);
private:
//     void orderPoints(float& x1, float& x2, float& y1, float& y2, float& d1, float& d2);
//     bool isSlopeBiggerThanOne(float x1, float x2, float y1, float y2) { return (std::fabs(y2 - y1) > std::fabs(x2 - x1)); }
    bool m_bBlurActive = false;
public:
    void DrawAxis();
    void DrawModelAxis();
    void SetGeneratedTexture(GENERATED_TEXTURE texture);
    GENERATED_TEXTURE GetGeneratedTexture();
};
