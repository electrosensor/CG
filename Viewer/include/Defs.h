#pragma once

#define DEFAULT_HEIGHT     720
#define DEFAULT_WIDTH      1280
#define MAX_HEIGHT_4K      2160
#define MAX_WIDTH_4K       3840
#define	DISABLED		   -1

#define I_MATRIX { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ 0,0,0,1 } }
#define SCALING_MATRIX(value) { {(value),0,0,0},{ 0,(value),0,0 },{ 0,0,(value),0 },{ 0,0,0,1 } }
#define TRANSLATION_MATRIX(x, y, z) { {1,0,0,0},{ 0,1,0,0 },{ 0,0,1,0 },{ (x),(y),(z),1 } }

#define WHITE_COLOR (1.0f, 1.0f, 1.0f)

typedef enum _RETURN_CODE
{
	RC_SUCCESS = 0,
	RC_FAILURE,
	RC_UNDEFINED,
	RC_IO_ERROR

}RETURN_CODE;