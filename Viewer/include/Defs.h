#pragma once

#define DEFAULT_HEIGHT     720
#define DEFAULT_WIDTH      1280
#define MAX_HEIGHT_4K      2160
#define MAX_WIDTH_4K       3840
#define	DISABLED		   -1

#define ZERO_MATRIX						 { {0,0,0,0},{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 } }
#define I_MATRIX						 { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define SCALING_MATRIX(value)			 { {(value),0,0,0},{ 0,(value),0,0 },{ 0,0,(value),0 },{ 0,0,0,1 } }
#define TRANSLATION_MATRIX(x, y, z)		 { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ (x),(y),(z),1 } }
#define HOMOGENIC_VECTOR4				 { 0.0, 0.0, 0.0, 1.0 }
#define ROTATING_MATRIX_X_AXIS(angle)	 { {1,0,0,0},{ 0,cos(angle),sin(angle),0 },{ 0,-sin(angle),cos(angle),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Y_AXIS(angle)	 { {cos(angle),0,-sin(angle),0},{ 0,1,0,0 },{ sin(angle),0,cos(angle),0 },{ 0,0,0,1 } }
#define ROTATING_MATRIX_Z_AXIS(angle)	 { {cos(angle),sin(angle),0,0},{ -sin(angle),cos(angle),0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }

#define WHITE_COLOR (1.0f, 1.0f, 1.0f)

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
	PM_BOX,

}PRIM_MODEL;