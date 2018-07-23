#pragma once

#define DEFAULT_HEIGHT     720
#define DEFAULT_WIDTH      1280
#define MAX_HEIGHT_4K      2160
#define MAX_WIDTH_4K       3840
#define	DISABLED		   -1

typedef enum _RETURN_CODE
{
	RC_SUCCESS = 0,
	RC_FAILURE,
	RC_UNDEFINED,
	RC_IO_ERROR

}RETURN_CODE;
