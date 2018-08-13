#include <algorithm>
#include "Renderer.h"
#include "InitShader.h"
#include <imgui/imgui.h>
#include "Util.h"
#include "Defs.h"


Renderer::Renderer() : m_width(DEFAULT_WIDTH), m_height(DEFAULT_HEIGHT)
{
    initOpenGLRendering();
    createBuffers(DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

Renderer::Renderer(int w, int h) : m_width(w), m_height(h), m_normalTransform(I_MATRIX), m_cameraTransform(I_MATRIX), m_objectTransform(I_MATRIX), m_cameraProjection(I_MATRIX), m_worldTransformation(I_MATRIX), m_bgColor(Util::getColor(CLEAR)), m_polygonColor(Util::getColor(WHITE))
{
    initOpenGLRendering();
    createBuffers(w, h);
}

Renderer::~Renderer()
{
    delete[] colorBuffer;
    delete[] zBuffer;
}


glm::vec4 Renderer::processPipeline(const glm::vec4& point, PIPE_TYPE pipeType /*= FULL*/)
{
    switch (pipeType)
    {
    case FULL:
        return m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * point;
    case AXIS:
        return m_cameraProjection * m_cameraTransform * point;
    case MODEL:
        return m_cameraProjection * m_cameraTransform * m_objectTransform * point;
    default:
        return HOMOGENEOUS_VECTOR4;
    }

    
}

void Renderer::DrawTriangles(const vector<glm::vec4>* vertices, bool bDrawFaceNormals /*= false*/, const glm::vec4* modelCentroid /*= NULL*/, float normScaleRate /*= 1*/, bool bIsCamera /*= false*/)
{
    vector<glm::vec4>::const_iterator it = vertices->begin();
    
    while (it != vertices->end())
    {
        glm::vec4 p1 = *(it++);
        if (it == vertices->end()) break;
        glm::vec4 p2 = *(it++);
        if (it == vertices->end()) break;
        glm::vec4 p3 = *(it++);

        glm::vec4 nrm1 = p1;
        glm::vec4 nrm2 = p2;
        glm::vec4 nrm3 = p3;

        p1 = processPipeline(p1);
        p2 = processPipeline(p2);
        p3 = processPipeline(p3);

        DrawLine(toViewPlane(p1), toViewPlane(p2), m_polygonColor);
        DrawLine(toViewPlane(p2), toViewPlane(p3), m_polygonColor);
        DrawLine(toViewPlane(p3), toViewPlane(p1), m_polygonColor);

        if (bDrawFaceNormals)
        {
            glm::vec4 subs1      = nrm3 - nrm1;
            glm::vec4 subs2      = nrm2 - nrm1;
                      subs1.w    = 1;
                      subs2.w    = 1;

            glm::vec4 faceNormal = Util::Cross(subs1, subs2);

            glm::vec4 faceCenter   = (nrm1 + nrm2 + nrm3) / 3.0f;
                      faceCenter.w = 1;

            glm::vec4 normalizedFaceNormal = Util::isVecEqual(faceNormal, glm::vec4(0, 0, 0, 1)) ? faceNormal : glm::normalize(faceNormal);

            normalizedFaceNormal = glm::mat4x4(SCALING_MATRIX4(normScaleRate)) * normalizedFaceNormal;

            //normalizedFaceNormal *= fnScale;
            glm::vec4 nP1 = processPipeline(faceCenter);
            glm::vec4 nP2 = processPipeline(faceCenter + normalizedFaceNormal);

            DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(LIME));
        }
    }
}

void Renderer::drawVerticesNormals(const vector<glm::vec4>& vertices, const vector<glm::vec4>& normals, float normScaleRate)
{
    for (int i = 0; i < normals.size() && i < vertices.size(); i++)
    {
        glm::vec4 vertex       = vertices[i];
        glm::vec4 vertexNormal = normals[i];

        glm::vec4 normalizedVertexNormal = Util::isVecEqual(vertexNormal, HOMOGENEOUS_VECTOR4) ? vertexNormal : glm::normalize(vertexNormal);

        normalizedVertexNormal = glm::mat4x4(SCALING_MATRIX4(normScaleRate)) * normalizedVertexNormal;

        glm::vec4 nP1 = processPipeline(vertex);
        glm::vec4 nP2 = processPipeline(vertex + normalizedVertexNormal);

        DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(RED));
    }
}

void Renderer::drawBordersCube(CUBE borderCube)
{
    for each (std::pair<glm::vec4, glm::vec4> line in borderCube.line)
    {
        glm::vec4 pStart = processPipeline(line.first);
        glm::vec4 pEnd   = processPipeline(line.second);

        DrawLine(toViewPlane(pStart), toViewPlane(pEnd), COLOR(BLUE));
    }
}

glm::vec2 Renderer::toViewPlane(const glm::vec4& pointParam)
{
    // convert to raster space 

    glm::vec3 point = Util::toCartesianForm(pointParam);

    glm::vec2 screenPoint;

    screenPoint.x = ((point.x + 1) * m_width  / 2.0f);
    screenPoint.y = ((point.y + 1) * m_height / 2.0f);

    screenPoint.x = round((screenPoint.x - (m_width  / 2.0f)) * (250.0f / m_width ) + (m_width  / 2.0f));
    screenPoint.y = round((screenPoint.y - (m_height / 2.0f)) * (250.0f / m_height) + (m_height / 2.0f));
    if (/*m_bzBuffer*/1)
    {
//             for every pixel(x, y);
//                 PutZ(x, y, MaxZ);
//             for each polygon P
//                 Q = Project(P);
//             for each pixel(x, y) in Q
//                 z : = Depth(Q, x, y);
//             if (z < GetZ(x, y)) then
//                 PutColor(x, y, Col(P));
//             PutZ(x, y, z);
//             end
//                 end
//                 end

    }
    return glm::vec2((int)screenPoint.x, (int)screenPoint.y);
}


void Renderer::SetCameraTransform(const glm::mat4x4 & cTransform)
{
    m_cameraTransform = cTransform;
}

void Renderer::SetProjection(const glm::mat4x4 & projection)
{
    m_cameraProjection = projection;
}

void Renderer::SetObjectMatrices(const glm::mat4x4 & oTransform, const glm::mat4x4 & nTransform)
{
    m_objectTransform = oTransform;
    m_normalTransform = nTransform;
}

void Renderer::putPixel(int i, int j, const glm::vec4& color)
{
    if (i < 0) return; if (i >= m_width) return;
    if (j < 0) return; if (j >= m_height) return;
    colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = color.x;
    colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = color.y;
    colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = color.z;
}

void Renderer::putZ(int x, int y, float d)
{
    if (x < 0) return; if (x >= m_width) return;
    if (y < 0) return; if (y >= m_height) return;
    zBuffer[Z_BUF_INDEX(m_width, x, y)] = d;
}

void Renderer::DrawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color)
{
    float dx, dy;

    float x1 = p1.x;
    float x2 = p2.x;
    float y1 = p1.y;
    float y2 = p2.y;
 
    const bool bSteep = isSlopeBiggerThanOne(x1, x2, y1, y2);
    
    orderPoints(x1, x2, y1, y2);

    getDeltas(x1, x2, y1, y2, &dx, &dy);
    
    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for (int x = (int)x1; x < maxX; x++)
    {
        putPixel(x, y, bSteep, color);

        yStepErrorUpdate(dx, dy, error, y, ystep);
    }
}

void Renderer::putPixel(int x, int y, bool steep, const glm::vec4& color)
{
    if (steep)
    {
        putPixel(y, x, color);
    }
    else
    {
        putPixel(x, y, color);
    }
}

void Renderer::createBuffers(int w, int h)
{
    createOpenGLBuffer(); //Do not remove this line.
    colorBuffer = new float[3*w*h];
    zBuffer = new float[w*h];
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            putPixel(i, j, HOMOGENEOUS_VECTOR4);
            putZ(i, j, 0.f);
        }
    }
}

void Renderer::setWorldTransformation(glm::mat4x4 worldTransformation)
{
    m_worldTransformation = worldTransformation;
}

void Renderer::orderPoints(float& x1, float& x2, float& y1, float& y2)
{
    if (isSlopeBiggerThanOne(x1, x2, y1, y2))
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
}

void Renderer::drawAxis()
{
    glm::vec4 axisX     = m_worldTransformation[0];
    glm::vec4 axisY     = m_worldTransformation[1];
    glm::vec4 axisZ     = m_worldTransformation[2];
              axisX.w   = 1;
              axisY.w   = 1;
              axisZ.w   = 1;

    glm::vec4 zeroPoint = HOMOGENEOUS_VECTOR4;

    axisX     = processPipeline(axisX,     AXIS);
    axisY     = processPipeline(axisY,     AXIS);
    axisZ     = processPipeline(axisZ,     AXIS);
    zeroPoint = processPipeline(zeroPoint, AXIS);

    axisX = glm::mat4x4(SCALING_MATRIX4(5))*axisX;
    axisY = glm::mat4x4(SCALING_MATRIX4(5))*axisY;
    axisZ = glm::mat4x4(SCALING_MATRIX4(5))*axisZ;


    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisX), COLOR(X_COL));
    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisY), COLOR(Y_COL));
    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisZ), COLOR(Z_COL));

}

void Renderer::drawModelAxis()
{
    glm::vec4 axisX = { 1,0,0,1 };
    glm::vec4 axisY = { 0,1,0,1 };
    glm::vec4 axisZ = { 0,0,1,1 };
    glm::vec4 zeroPoint = { m_objectTransform[0][3],m_objectTransform[1][3],m_objectTransform[2][3] , 1 };

    axisX     = processPipeline(axisX,     MODEL);
    axisY     = processPipeline(axisY,     MODEL);
    axisZ     = processPipeline(axisZ,     MODEL);
    zeroPoint = processPipeline(zeroPoint, MODEL);



    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisX*5.f), COLOR(X_COL));
    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisY*5.f), COLOR(Y_COL));
    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisZ*5.f), COLOR(Z_COL));

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
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
    glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );
    // Same for texture coordinates data.
    GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
    glEnableVertexAttribArray( vTexCoord );
    glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

    //glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

    // Tells the shader to use GL_TEXTURE0 as the texture id.
    glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::getDeltas(IN float x1, IN float x2, IN float y1, IN float y2, OUT float* pDx, OUT float* pDy)
{
    *pDx = x2 - x1;
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

void Renderer::createOpenGLBuffer()
{
    // Makes GL_TEXTURE0 the current active texture unit
    glActiveTexture(GL_TEXTURE0);
    // Makes glScreenTex (which was allocated earlier) the current texture.
    glBindTexture(GL_TEXTURE_2D, glScreenTex);
    // malloc for a texture on the gpu.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
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
            putPixel(i, j, m_bgColor);
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


glm::vec4 Renderer::GetBgColor()
{
    return m_bgColor;
}

void Renderer::SetBgColor(glm::vec4 newBgColor)
{
    m_bgColor = newBgColor;
}

glm::vec4 Renderer::GetPolygonColor()
{
    return m_polygonColor;
}

void Renderer::SetPolygonColor(glm::vec4 newMeshColor)
{
    m_polygonColor = newMeshColor;
}

void Renderer::ClearDepthBuffer()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            putZ(i, j, 0.f);
        }
    }
}