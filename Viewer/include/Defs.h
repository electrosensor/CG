#pragma once

#include <string>

#define DEFAULT_HEIGHT                   720
#define DEFAULT_WIDTH                    1280
#define MAX_HEIGHT_4K                    2160
#define MAX_WIDTH_4K                     3840

#define DISABLED                         -1
#define PI                               3.141592653589793238462643383279502884L
#define INDEX(width,x,y,c)               ((x)+(y)*(width))*3+(c)
#define FACE_ELEMENTS                    3
#define TO_RADIAN(angle)                 ((angle) * PI / 180.0f)
#define ZERO_MATRIX                      { {0,0,0,0},{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } }
#define FLATTEN_MATRIX                   { {1,0,0,0},{ 0,1,0,0 },{ 0,0,0,0 },{ 0,0,0,1 } }
#define I_MATRIX                         { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define SCALING_MATRIX(value)            { {(value),0,0,0},{ 0,(value),0,0 },{ 0,0,(value),0 },{ 0,0,0,1 } }
#define TRANSLATION_MATRIX(x, y, z)      { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ (x),(y),(z),1 } }
#define HOMOGENEOUS_VECTOR4              { 0.0f, 0.0f, 0.0f, 1.0f }
#define HOMOGENEOUS_MATRIX4(x, y, z, w)  { {(x),0,0,0},{ 0,(y),0,0 },{ 0,0,(z),0 },{ 0,0,0,(w) } }
#define ROTATING_MATRIX_X_AXIS(angle)    { {1,0,0,0},{ 0,cos(angle),sin(angle),0 },{ 0,-sin(angle),cos(angle),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Y_AXIS(angle)    { {cos(angle),0,-sin(angle),0},{ 0,1,0,0 },{ sin(angle),0,cos(angle),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Z_AXIS(angle)    { {cos(angle),sin(angle),0,0},{ -sin(angle),cos(angle),0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define PERSPECTIVE_MATRIX(d)            { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,1.0f/(d) },{ 0,0,0,0 } }

#define WHITE_COLOR                      (1.0f, 1.0f, 1.0f)

#define SET_PROJ_PARAMS(projParams)                                            \
                                         float left   = projParams.left;       \
                                         float right  = projParams.right;      \
                                         float top    = projParams.top;        \
                                         float bottom = projParams.bottom;     \
                                         float zNear  = projParams.zNear;      \
                                         float zFar   = projParams.zFar;       \

#define SET_PERSP_PARAMS(perspParams)                                          \
                                         float fovy     = perspParams.fovy;    \
                                         float aspect   = perspParams.aspect;  \
                                         float zNear    = perspParams.zNear;   \
                                         float zFar     = perspParams.zFar;    \


typedef enum _RETURN_CODE
{
    RC_SUCCESS = 0,
    RC_FAILURE,
    RC_UNDEFINED,
    RC_IO_ERROR

}RETURN_CODE;

typedef enum _PRIM_MODEL
{
    PM_SPHERE = 0,
    PM_CUBE,

}PRIM_MODEL;

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
    float fovy;
    float aspect;
    float zNear;
    float zFar;
}PERSPECTIVE_PARAMS, *PPERSPECTIVE_PARAMS;
