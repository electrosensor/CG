#pragma once

#define DEFAULT_HEIGHT			 720
#define DEFAULT_WIDTH			 1280
#define MAX_HEIGHT_4K			 2160
#define MAX_WIDTH_4K			 3840

#define	DISABLED			 -1
#define PI				                 3.141592653589793238462643383279502884
#define INDEX(width,x,y,c)		         ((x)+(y)*(width))*3+(c)
#define FACE_ELEMENTS 			         3
#define TO_RADIAN(angle)		         ((angle)*2.0f*PI / 360.0f)
#define ZERO_MATRIX                      { {0,0,0,0},{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } }
#define FLATTEN_MATRIX                   { {1,0,0,0},{ 0,1,0,0 },{ 0,0,0,0 },{ 0,0,0,1 } }
#define I_MATRIX                         { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define SCALING_MATRIX(value)            { {(value),0,0,0},{ 0,(value),0,0 },{ 0,0,(value),0 },{ 0,0,0,1 } }
#define TRANSLATION_MATRIX(x, y, z)      { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ (x),(y),(z),1 } }
#define HOMOGENIC_VECTOR4                { 0.0f, 0.0f, 0.0f, 1.0f }
#define ROTATING_MATRIX_X_AXIS(angle)	 { {1,0,0,0},{ 0,cos(TO_RADIAN(angle)),sin(TO_RADIAN(angle)),0 },{ 0,-sin(TO_RADIAN(angle)),cos(TO_RADIAN(angle)),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Y_AXIS(angle)	 { {cos(TO_RADIAN(angle)),0,-sin(TO_RADIAN(angle)),0},{ 0,1,0,0 },{ sin(TO_RADIAN(angle)),0,cos(TO_RADIAN(angle)),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Z_AXIS(angle)	 { {cos(TO_RADIAN(angle)),sin(TO_RADIAN(angle)),0,0},{ -sin(TO_RADIAN(angle)),cos(TO_RADIAN(angle)),0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define WHITE_COLOR			             (1.0f, 1.0f, 1.0f)

#define SET_PROJ_PARAMS(projParams)                                          \
                                         float right  = projParams.right;    \
                                         float left   = projParams.left;     \
                                         float top    = projParams.top;      \
                                         float bottom = projParams.bottom;   \
                                         float zNear  = projParams.zNear;    \
                                         float zFar   = projParams.zFar;     \

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
