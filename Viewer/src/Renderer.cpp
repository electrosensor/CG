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

Renderer::Renderer(int w, int h) : m_width(w), m_height(h)
{
    initOpenGLRendering();
    createBuffers(w,h);
}

Renderer::~Renderer()
{
    delete[] colorBuffer;
}

void Renderer::DrawTriangles(const vector<glm::vec3>* vertices, bool bDrawFaceNormals /*= false*/, UINT32 normScaleRate /*= 1*/, const vector<glm::vec3>* normals/*=NULL*/)
{
    vector<glm::vec3>::const_iterator it = vertices->begin();
    while (it != vertices->end())
    {
        glm::vec3 p1 = *(it++);
        if(it == vertices->end()) break;
        glm::vec3 p2 = *(it++);
        if (it == vertices->end()) break;
        glm::vec3 p3 = *(it++);

        glm::vec3 nrm1 = p1;
        glm::vec3 nrm2 = p2;
        glm::vec3 nrm3 = p3;

        p1 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(p1));
        p2 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(p2));
        p3 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(p3));

        DrawLine(toViewPlane(p1), toViewPlane(p2), COLOR(WHITE));
        DrawLine(toViewPlane(p2), toViewPlane(p3), COLOR(WHITE));
        DrawLine(toViewPlane(p3), toViewPlane(p1), COLOR(WHITE));

        if (bDrawFaceNormals)
        {
            glm::vec3 subs1      = nrm3 - nrm1;
            glm::vec3 subs2      = nrm2 - nrm1;
            glm::vec3 faceNormal = glm::cross(subs1, subs2);
            
            glm::vec3 faceCenter        = (nrm1 + nrm2 + nrm3) / 3.0f;

            glm::vec3 normalizedFaceNormal = glm::normalize(faceNormal);
             
            glm::vec3 nP1 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(faceCenter));
            glm::vec3 nP2 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(faceCenter + normalizedFaceNormal));

            DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(LIME));
        }
    }
}

void Renderer::drawVerticesNormals(const vector<glm::vec3>& vertices, const vector<glm::vec3>& normals)
{
    for (int i = 0; i < normals.size() && i < vertices.size(); i++)
    {
        glm::vec3 vertex       = vertices[i];
        glm::vec3 vertexNormal = normals[i];

        glm::vec3 nP1 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(vertex));
        glm::vec3 nP2 = Util::toCartesianForm(m_cameraProjection * m_cameraTransform * m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(vertex + vertexNormal));

        DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(RED));
    }
}

void Renderer::drawBordersCube(CUBE_LINES borderCube, glm::vec3 modelOffset)
{
    for each (std::pair<glm::vec3, glm::vec3> line in borderCube.line)
    {
        glm::vec3 pStart = Util::toCartesianForm(m_cameraProjection * m_cameraTransform *  m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(line.first));
        glm::vec3 pEnd   = Util::toCartesianForm(m_cameraProjection * m_cameraTransform *  m_worldTransformation * m_objectTransform * Util::toHomogeneousForm(line.second));

        DrawLine(toViewPlane(pStart), toViewPlane(pEnd), COLOR(RED));
    }
}

glm::uvec2 Renderer::toViewPlane(const glm::vec2& point)
{
    // convert to raster space 

    glm::vec2 screenPoint;

    screenPoint.x = ((point.x + 1) * m_width  / 2.0f);
    screenPoint.y = ((point.y + 1) * m_height / 2.0f);

    screenPoint.x = round((screenPoint.x - (m_width  / 2.0f)) * (500.0f / m_width ) + (m_width  / 2.0f));
    screenPoint.y = round((screenPoint.y - (m_height / 2.0f)) * (500.0f / m_height) + (m_height / 2.0f));

    return glm::vec2(screenPoint.x, screenPoint.y);
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

void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
    if (i < 0) return; if (i >= m_width) return;
    if (j < 0) return; if (j >= m_height) return;
    colorBuffer[INDEX(m_width, i, j, 0)] = color.x;
    colorBuffer[INDEX(m_width, i, j, 1)] = color.y;
    colorBuffer[INDEX(m_width, i, j, 2)] = color.z;
}

void Renderer::DrawLine(const glm::uvec2& p1, const glm::uvec2& p2, const glm::vec3& color)
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

void Renderer::putPixel(int x, int y, bool steep, const glm::vec3& color)
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
    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            putPixel(i, j, glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
}

void Renderer::SetDemoBuffer()
{
    int r = 5;
    // Wide red vertical line
    glm::vec4 red = glm::vec4(1, 0, 0, 1);
    for (int i = 0; i<m_height; i++)
    {
        for (int r0 = 0; r0 < r; r0++)
        {
            putPixel((m_width / 2) + r0, i, red);
            putPixel((m_width / 2) - r0, i, red);
        }
    }
    // Wide magenta horizontal line
    glm::vec4 magenta = glm::vec4(1, 0, 1, 1);
    for (int i = 0; i<m_width; i++)
    {
        for (int r0 = 0; r0 < r; r0++)
        {
            putPixel(i, (m_height / 2) + r0, magenta);
            putPixel(i, (m_height / 2) - r0, magenta);
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

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            putPixel(i, j, color);
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
    colorBuffer = new float[3 * h*w];
    createOpenGLBuffer();
}
