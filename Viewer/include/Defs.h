#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include "glm/common.hpp"
#include <glm/gtc/matrix_transform.hpp>


#define VIEW_SCALING                     300.f

#define DEFAULT_HEIGHT                   720
#define DEFAULT_WIDTH                    1280
#define MAX_HEIGHT_4K                    2160
#define MAX_WIDTH_4K                     3840

#define CAMERA_OBJ_FILE                  "PrimModels/camera.obj"
#define LIGHT_OBJ_FILE                   "PrimModels/sphere_test.obj"

#define DISABLED                         -1
#define PI                               3.141592653589793238462643383279502884L
#define COLOR_BUF_INDEX(width,x,y,c)     ((x)+(y)*(width))*3+(c)
#define Z_BUF_INDEX(width,x,y)           ((x)+(y)*(width))
#define FACE_ELEMENTS                    3
#define TO_RADIAN(angle)                 ((angle) * PI / 180.0f)
#define ZERO_MATRIX                      { {0,0,0,0},{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } }
#define ZERO_VEC3                        {0,0,0}
#define ZERO_VEC4                        {0,0,0,0}
#define I_MATRIX                         { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define SCALING_MATRIX4(value)           { {(value),0,0,0},{ 0,(value),0,0 },{ 0,0,(value),0 },{ 0,0,0,1 } }
#define TRANSLATION_MATRIX(x, y, z)      { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ (x),(y),(z),1 } }
#define HOMOGENEOUS_VECTOR4              { 0.0f, 0.0f, 0.0f, 1.0f }
#define HOMOGENEOUS_MATRIX4(x, y, z, w)  { {(x),0,0,0},{ 0,(y),0,0 },{ 0,0,(z),0 },{ 0,0,0,(w) } }
#define ROTATING_MATRIX_X_AXIS(angle)    { {1,0,0,0},{ 0,cos(angle),sin(angle),0 },{ 0,-sin(angle),cos(angle),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Y_AXIS(angle)    { {cos(angle),0,-sin(angle),0},{ 0,1,0,0 },{ sin(angle),0,cos(angle),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Z_AXIS(angle)    { {cos(angle),sin(angle),0,0},{ -sin(angle),cos(angle),0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define PERSPECTIVE_MATRIX(d)            { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,1.0f/(d) },{ 0,0,0,0 } }
#define DEFAULT_CAMERA_POSITION          { 2.f, 2.f, 2.f }, { 0, 0, 0 }, { 0, 1, 0, }

#define MAX(a,b)                         (((a) > (b)) ? (a) : (b))
#define MAX3(a,b,c)                      (MAX(a,MAX(b,c)))
#define MIN(a,b)                         (((a) < (b)) ? (a) : (b))
#define MIN3(a,b,c)                      (MIN(a,MIN(b,c)))
#define NORM_ZERO_TO_ONE(value,min,max)  ((value) - (min)) / ((max) - (min))
#define NORMALIZE_COORDS(value,min,max)  (((NORM_ZERO_TO_ONE(value,min,max)*2) - 1))

#define TUPLE_POLYGONS                   (0)
#define TUPLE_VERTICES                   (1)
#define TUPLE_VNORMALS                   (2)
#define TUPLE_VPOSITIONS                 (3)
#define TUPLE_TEXTURES                   (4)

 enum R_COLOR
{
    CLEAR,
    WHITE,
    BLACK,
    GREEN,
    LIME,
    BLUE,
    RED,
    YELLOW,
    X_COL,
    Y_COL,
    Z_COL,
    YURI_BG,
    YURI_POLYGON,
    YURI_WIRE
};


enum AXES
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
    AXIS_XY,
    AXIS_YZ,
    AXIS_ZX
};

enum ROTATION_REL
{
    RR_WORLD,
    RR_SELF
};

enum FRAME_TYPE
{
    FT_CAMERA,
    FT_MODEL,
    FT_WORLD,
    FT_LIGHT
};

enum SHADING_TYPE
{
    ST_NO_SHADING,
    ST_SOLID,
    ST_FLAT,
    ST_PHONG,
    ST_GOURAUD
};

enum LIGHT_TYPE
{
    LT_AMBIENT,
    LT_SPECULAR,
    LT_DIFFUSIVE
};

enum LIGHT_SOURCE_TYPE
{
    LST_POINT,
    LST_PARALLEL,
    LST_AREA
};

enum GENERATED_TEXTURE
{
    GT_NONE,
    GT_CRYSTAL,
    GT_RUG
};

#define SET_PROJ_PARAMS(projParams)                                           \
                                         float left      = projParams.left;   \
                                         float right     = projParams.right;  \
                                         float top       = projParams.top;    \
                                         float bottom    = projParams.bottom; \
                                         float zNear     = projParams.zNear;  \
                                         float zFar      = projParams.zFar;   \

#define INIT_CUBE_COORDS(maxCoords, minCoords)                                \
                                         float cRight    = m_maxCoords.x;     \
                                         float cTop      = m_maxCoords.y;     \
                                         float cFar      = m_maxCoords.z;     \
                                         float cLeft     = m_minCoords.x;     \
                                         float cBottom   = m_minCoords.y;     \
                                         float cNear     = m_minCoords.z;     \


#define SET_PERSP_PARAMS(perspParams)                                         \
                                         float fovy     = perspParams.fovy;   \
                                         float aspect   = perspParams.aspect; \
                                         float zNear    = perspParams.zNear;  \
                                         float zFar     = perspParams.zFar;   \


enum RETURN_CODE
{
    RC_SUCCESS = 0,
    RC_FAILURE,
    RC_UNDEFINED,
    RC_IO_ERROR

};

enum PRIM_MODEL
{
    PM_SPHERE = 0,
    PM_CUBE,
    PRV_CAMERA,
    PM_TEAPOT

};

typedef struct _PROJ_PARAMS
{
    float left;
    float right;
    float bottom;
    float top;
    float zNear;
    float zFar;

}PROJ_PARAMS, *PPROJ_PARAMS;

typedef struct _PERSPECTIVE_PARAMS
{
    GLfloat fovy;
    GLfloat aspect;
    GLfloat zNear;
    GLfloat zFar;
}PERSPECTIVE_PARAMS, *PPERSPECTIVE_PARAMS;

typedef struct _CUBE
{
    std::pair<glm::vec3, glm::vec3> lines[12];
}CUBE, *PCUBE;

typedef enum _PIPE_TYPE
{
    FULL = 0,
    AXIS,
    MODEL,
    LIGHT

}PIPE_TYPE, *PPIPE_TYPE;

typedef enum _POST_EFFECT
{
    NONE = 0,
    BLUR_SCENE,
    BLOOM
}POST_EFFECT, *PPOST_EFFECT;

// 
// typedef struct _GUI_CONFIG
// {
//     //Main menu
//     bool showDemoWindow = false;
//     bool modelControlWindow = true;
//     bool bModelControlFrame = false;
// 
//     bool lightingMenu = true;
//     bool bLightControlFrame = false;
// 
//     bool colorMenu = false;
//     bool showFile = false;
// 
//     int world[3] = { 1,1,1 };
//     glm::mat4x4 worldTransformation = ZERO_MATRIX;
// 
//     //Cameras:
//     float eye[3] = { 2,2,2 };
//     float at[3] = { 0,0,0 };
// 
//     bool bShowCamera = false;
// 
//     int cameraIndex = 0;
//     mat4x4 activeCameraTransformation = ZERO_MATRIX;
// 
//     //Camera controls:
//     int currentFrame = FRAME_TYPE::FT_CAMERA;
//     AXES currentAxis = AXES::AXIS_Y;
//     int currentRel = ROTATION_REL::RR_WORLD;
// 
//     float rotAngle = PI / 36.0f;
// 
//     //camera scaling:
//     float camScaleFactor = 1.5f;
// 
//     float moveFactor = 0.5f;
// 
//     //Projection:
//     PROJ_PARAMS projParams =
//         {
//             projParams.left = -1,
//             projParams.right = 1,
//             projParams.bottom = -1,
//             projParams.top = 1,
//             projParams.zNear = 0.1f,
//             projParams.zFar = 2,
//         };
// 
// }GUI_CONFIG, *PGUI_CONFIG;
// 

