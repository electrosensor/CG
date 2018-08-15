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


vec4 Renderer::processPipeline(const vec4& point, PIPE_TYPE pipeType /*= FULL*/)
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

        DrawLine(toViewPlane(p1), toViewPlane(p2), m_polygonColor);
        DrawLine(toViewPlane(p2), toViewPlane(p3), m_polygonColor);
        DrawLine(toViewPlane(p3), toViewPlane(p1), m_polygonColor);

        maxX = MAX(maxX, (MAX(p1.x, p2.x)));
        maxY = MAX(maxY, (MAX(p1.y, p2.y)));
        maxZ = MAX(maxZ, (MAX(p1.z, p2.z)));

        if (bDrawFaceNormals)
        {

            auto nrm1_3 = Util::toCartesianForm(nrm1);
            auto nrm2_3 = Util::toCartesianForm(nrm2);
            auto nrm3_3 = Util::toCartesianForm(nrm3);

            auto subs1 = nrm3_3 - nrm1_3;
            auto subs2 = nrm2_3 - nrm1_3;

            auto faceNormal = cross(subs1, subs2);

            auto faceCenter   = (nrm1_3 + nrm2_3 + nrm3_3) / 3.0f;

            auto normalizedFaceNormal = Util::isVecEqual(faceNormal, vec3(0)) ? faceNormal : normalize(faceNormal);

            normalizedFaceNormal.x *= normScaleRate;
            normalizedFaceNormal.y *= normScaleRate;
            normalizedFaceNormal.z *= normScaleRate;

            auto nP1 = processPipeline(Util::toHomogeneousForm(faceCenter));
            auto nP2 = processPipeline(Util::toHomogeneousForm(faceCenter + normalizedFaceNormal));

            DrawLine(toViewPlane(nP1), toViewPlane(nP2), COLOR(LIME));
        }

        //printf("max: %f\n%f\n%f\n%f\n%f\n%f\n", p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
    }
}

void Renderer::drawVerticesNormals(const vector<vec4>& vertices, const vector<vec4>& normals, float normScaleRate)
{
    for (int i = 0; i < normals.size() && i < vertices.size(); i++)
    {
        auto vertex       = Util::toCartesianForm(vertices[i]);
        auto vertexNormal = Util::toCartesianForm(normals[i]);

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

glm::vec3 Renderer::toViewPlane(const glm::vec4& point)
{
    // convert to raster space 
    vec3 screenPoint;

    screenPoint.x = ((point.x + 1) * m_width  / 2.0f);
    screenPoint.y = ((point.y + 1) * m_height / 2.0f);
 //   screenPoint.z = ((point.z + 1) * (m_height + m_width) / 2.0f);

    screenPoint.x = round((screenPoint.x - (m_width  / 2.0f)) * (250.0f / m_width ) + (m_width  / 2.0f));
    screenPoint.y = round((screenPoint.y - (m_height / 2.0f)) * (250.0f / m_height) + (m_height / 2.0f));
 //   screenPoint.z = round((screenPoint.z - ((m_height + m_width) / 2.0f)) * (250.0f / m_height + m_width)) + ((m_height + m_width) / 2.0f);
    if (/*m_bzBuffer*/true)
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
    return vec3((int)screenPoint.x, (int)screenPoint.y, (int)screenPoint.z);
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

void Renderer::putPixel(int i, int j, /*float d,*/ const vec4& color)
{
    if (i < 0) return; if (i >= m_width) return;
    if (j < 0) return; if (j >= m_height) return;
//     if (putZ(i, j, d))
//     {
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 0)] = color.x;
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 1)] = color.y;
        colorBuffer[COLOR_BUF_INDEX(m_width, i, j, 2)] = color.z;
/*    }*/

}

bool Renderer::putZ(int x, int y, float d)
{
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return false;
    }
    
    if (zBuffer[Z_BUF_INDEX(m_width, x, y)] - d > std::numeric_limits<float>::epsilon())
    {
//               fprintf(stderr, "FALSE - x,y:%d,%d Index is %d, zBuffer value is %f, depth value is %f\n", x, y, Z_BUF_INDEX(m_width, x, y), zBuffer[Z_BUF_INDEX(m_width, x, y)], d);
//         printf("zBuffer for (%d,%d) = %f, and depth is %f\n", x, y, zBuffer[Z_BUF_INDEX(m_width, x, y)], d);
        return false;
    }
    zBuffer[Z_BUF_INDEX(m_width, x, y)] = d;
//     fprintf(stderr, "TRUE - x,y:%d,%d Index is %d, zBuffer value is %f, depth value is %f\n", x, y, Z_BUF_INDEX(m_width, x, y), zBuffer[Z_BUF_INDEX(m_width, x, y)], d);
    return true;
}

void Renderer::DrawLine(const glm::ivec3& p1, const glm::ivec3& p2, const glm::vec4& color)
{

    int x0 = p1.x;
    int y0 = p1.y;
    int x1 = p2.x;
    int y1 = p2.y;
//     int z1 = p2.z;
//     int z0 = p1.z;
    int resSize = 1;
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? resSize : -resSize;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? resSize : -resSize;
//     int dz = abs(z1 - z0);
//     int sz = z0 < z1 ? resSize : -resSize;
    int dm = MAX(dx, /*MAX(*/dy/*, dz)*/), i = dm;
    x1 = y1 /*= z1*/ = dm / 2;

    for (; ; )
    {
        putPixel(x0, y0, /*z0,*/ color); //Printing points here
        if (i <= 0) break;
        x1 -= dx; if (x1 < 0) { x1 += dm; x0 += sx; }
        y1 -= dy; if (y1 < 0) { y1 += dm; y0 += sy; }
/*        z1 -= dz; if (z1 < 0) { z1 += dm; z0 += sz; }*/
        i -= resSize;
    }

//     float dd, dy, dx;
// 
//           float x1 = p1.x;
//           float x2 = p2.x;
//           
//           float y1 = p1.y;
//           float y2 = p2.y;
//       
//           float d1 = p1.z;
//           float d2 = p2.z;
//        
//           const bool bSteep = isSlopeBiggerThanOne(x1, x2, y1, y2);
//           
//           orderPoints(x1, x2, y1, y2, d1, d2);
//       
//           getDeltas(x1, x2, y1, y2, d1, d2, &dx, &dy, &dd);
//           
//           float errorX = dx / 2.0f;
//           float errorY = dy / 2.0f;
//           float errorD = dd / 2.0f;
// 
//           const int xstep = (x1 < x2) ? 1 : -1;
//           const int ystep = (y1 < y2) ? 1 : -1;
//           const int dstep = (d1 < d2) ? 1 : -1;
// 
//           auto y = (int)y1;
//           auto d = (int)d1;
//           auto x = (int)x1;
//       
//           const auto maxX = MAX(x2,MAX( y2, d2));
//       
//           for (auto x = (int)x1; x < maxX; x++)
//           {
//               putPixel(x, y, d, bSteep, color);
//       
//               yStepErrorUpdate(dx, dy, errorX, y, ystep);
//           }
//     int x1 = p1.x;
//     int x2 = p2.x;
//     int y1 = p1.y;
//     int y2 = p2.y;
//     float d1 = p1.z;
//     float d2 = p2.z;
// 
//     int dx = abs(x2 - x1);
//     int dy = abs(y2 - y1);
//     int dz = abs(d2 - d1);
// 
// 
//     int stepX = x1 < x2 ? 1 : -1;
//     int stepY = y1 < y2 ? 1 : -1;
//     int stepZ = d1 < d2 ? 1 : -1;
// 
//     int dMax = MAX(dx, MAX(dy, dz));
// 
// 
//     x1 = y1 = d1 = dMax / 2;
// 
//            for (int i = dMax;;) {  /* loop */
//                putPixel(x1, y1, d1, color);
//                if (i-- == 0) break;
//                if (x1 < 0) { x2 += dMax; x1 += stepX; }
//                if (y1 < 0) { y2 += dMax; y1 += stepY; }
//                if (d1 < 0) { d2 += dMax; d1 += stepZ; }
//            }

//     for (int i = 0; i < dMax; i++)
//     {
//         putPixel(x1, y1, d1, color);
//         if (x1 < 0) { x2 += dMax; x1 += stepX; }
//         if (y1 < 0) { y2 += dMax; y1 += stepY; }
//         if (d1 < 0) { d2 += dMax; d1 += stepZ; }
//     }
// //     }
// 
//     
//    
//     void plotLine3d(int x0, int y0, int z0, int x1, int y1, int z1)
//     {
//         int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
//         int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
//         int dz = abs(z1 - z0), sz = z0 < z1 ? 1 : -1;
//         int dm = max(dx, dy, dz), i = dm; /* maximum difference */
//         x1 = y1 = z1 = dm / 2; /* error offset */
// 
//         for (;;) {  /* loop */
//             setPixel(x0, y0, z0);
//             if (i-- == 0) break;
//              if (x1 < 0) { x1 += dm; x0 += sx; }
//              if (y1 < 0) { y1 += dm; y0 += sy; }
//              if (z1 < 0) { z1 += dm; z0 += sz; }
//         }
//     }


    /*
    

   if ~exist('precision','var') | isempty(precision) | round(precision) == 0
      precision = 0;
      P1 = round(P1);
      P2 = round(P2);
   else
      precision = round(precision);
      P1 = round(P1*(10^precision));
      P2 = round(P2*(10^precision));
   end

   d = max(abs(P2-P1)+1);
   X = zeros(1, d);
   Y = zeros(1, d);
   Z = zeros(1, d);

   x1 = P1(1);
   y1 = P1(2);
   z1 = P1(3);

   x2 = P2(1);
   y2 = P2(2);
   z2 = P2(3);

   dx = x2 - x1;
   dy = y2 - y1;
   dz = z2 - z1;

   ax = abs(dx)*2;
   ay = abs(dy)*2;
   az = abs(dz)*2;

   sx = sign(dx);
   sy = sign(dy);
   sz = sign(dz);

   x = x1;
   y = y1;
   z = z1;
   idx = 1;

   if(ax>=max(ay,az))			% x dominant
      yd = ay - ax/2;
      zd = az - ax/2;

      while(1)
         X(idx) = x;
         Y(idx) = y;
         Z(idx) = z;
         idx = idx + 1;

         if(x == x2)		% end
            break;
         end

         if(yd >= 0)		% move along y
            y = y + sy;
            yd = yd - ax;
         end

         if(zd >= 0)		% move along z
            z = z + sz;
            zd = zd - ax;
         end

         x  = x  + sx;		% move along x
         yd = yd + ay;
         zd = zd + az;
      end
   elseif(ay>=max(ax,az))		% y dominant
      xd = ax - ay/2;
      zd = az - ay/2;

      while(1)
         X(idx) = x;
         Y(idx) = y;
         Z(idx) = z;
         idx = idx + 1;

         if(y == y2)		% end
            break;
         end

         if(xd >= 0)		% move along x
            x = x + sx;
            xd = xd - ay;
         end

         if(zd >= 0)		% move along z
            z = z + sz;
            zd = zd - ay;
         end

         y  = y  + sy;		% move along y
         xd = xd + ax;
         zd = zd + az;
      end
   elseif(az>=max(ax,ay))		% z dominant
      xd = ax - az/2;
      yd = ay - az/2;

      while(1)
         X(idx) = x;
         Y(idx) = y;
         Z(idx) = z;
         idx = idx + 1;

         if(z == z2)		% end
            break;
         end

         if(xd >= 0)		% move along x
            x = x + sx;
            xd = xd - az;
         end

         if(yd >= 0)		% move along y
            y = y + sy;
            yd = yd - az;
         end

         z  = z  + sz;		% move along z
         xd = xd + ax;
         yd = yd + ay;
      end
   end

   if precision ~= 0
      X = X/(10^precision);
      Y = Y/(10^precision);
      Z = Z/(10^precision);
   end

   return;
    
    
    */
//     float dx, dy, dd;
// 
//     float x1 = p1.x;
//     float x2 = p2.x;
//     
//     float y1 = p1.y;
//     float y2 = p2.y;
// 
//     float d1 = p1.z;
//     float d2 = p2.z;
//  
//     const bool bSteep = isSlopeBiggerThanOne(x1, x2, y1, y2);
//     
//     orderPoints(x1, x2, y1, y2);
// 
//     getDeltas(x1, x2, y1, y2, d1, d2, &dx, &dy, &dd);
//     
//     float errorX = dx / 2.0f;
//     float errorD = dd / 2.0f;
//     const int ystep = (y1 < y2) ? 1 : -1;
//     const int dstep = (d1 < d2) ? 1 : -1;
//     auto y = (int)y1;
//     auto d = (int)d1;
// 
//     const auto maxX = (int)x2;
// 
//     for (auto x = (int)x1; x < maxX; x++)
//     {
//         putPixel(x, y, d, bSteep, color);
// 
//         yStepErrorUpdate(dx, dy, errorX, y, ystep);
//         yStepErrorUpdate(dx, dd, errorD, d, dstep);
//     }
}

void Renderer::putPixel(int x, int y, bool steep, /*float d,*/ const vec4& color)
{
    if (steep)
    {
        putPixel(y, x, /*d,*/ color);
    }
    else
    {
        putPixel(x, y, /*d,*/ color);
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
            putPixel(i, j, /*std::numeric_limits<float>::lowest(),*/ HOMOGENEOUS_VECTOR4);
        }
    }

    for (int i = 0; i < w; i++)
    {
        for (int j = 0; j < h; j++)
        {
            zBuffer[Z_BUF_INDEX(m_width, i, j)] = std::numeric_limits<float>::lowest();
        }
    }

}

void Renderer::setWorldTransformation(mat4x4 worldTransformation)
{
    m_worldTransformation = worldTransformation;
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
//     axisX = mat4x4(SCALING_MATRIX4(5))*axisX;
//     axisY = mat4x4(SCALING_MATRIX4(5))*axisY;
//     axisZ = mat4x4(SCALING_MATRIX4(5))*axisZ;


    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisX), COLOR(X_COL));
    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisY), COLOR(Y_COL));
    DrawLine(toViewPlane(zeroPoint), toViewPlane(axisZ), COLOR(Z_COL));

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

void Renderer::SetBgColor(vec4 newBgColor)
{
    m_bgColor = newBgColor;
}

vec4 Renderer::GetPolygonColor()
{
    return m_polygonColor;
}

void Renderer::SetPolygonColor(vec4 newMeshColor)
{
    m_polygonColor = newMeshColor;
}

void Renderer::ClearDepthBuffer()
{
    for (int i = 0; i < m_width; i++)
    {
        for (int j = 0; j < m_height; j++)
        {
            putZ(i, j, numeric_limits<float>::lowest());
        }
    }
}