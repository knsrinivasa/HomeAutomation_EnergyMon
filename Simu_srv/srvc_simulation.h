#include "Base/basic_types.h"

#include "srvc_add.h"
#include "srvc_bitmpl.h"
#include "srvc_div.h"
#include "srvc_mul.h"
#include "srvc_misc.h"
#include "srvc_div.h"
#include "srvc_mul.h"
#include "srvc_sub.h"

/**************************************************************
*       Required for Service library implementation
**************************************************************/

typedef unsigned int   uint;
typedef signed int     sint;
typedef double         real;

typedef signed char    sint8;
    
typedef signed short   sint16;
    
typedef signed long    sint32;
    
typedef float          real32;
typedef double         real64;


typedef struct
{
#if ((defined (HIGH_BYTE_FIRST)) && (HIGH_BYTE_FIRST))
	uint32 h;
	uint32 l;
#else
	uint32 l;
	uint32 h;
#endif
} uint64s;

typedef union
{
	uint64    u64;
	uint64s   u64s;
} uint64u;


/*
*********************************************************************************************************
*                                       GLOBAL  DEFINITIONS (GENERIC FORMAT)
*********************************************************************************************************
*/

#define MAXSINT8	0X7F
#define MAXSINT16	0X7FFF
#define MAXSINT32	0X7FFFFFFF
#define MAXSINT64	0X7FFFFFFFFFFFFFFF


#ifndef MAXUINT8
#define MAXUINT8	0XFF
#endif

#ifndef MAXUINT16
#define MAXUINT16	0XFFFF
#endif

#ifndef MAXUINT32
#define MAXUINT32	0XFFFFFFFF
#endif

#define MINSINT8	-128
#define MINSINT16	-32768
#define MINSINT32	-2147483648
#define MINSINT64	-9223372036854775808
                        
#define MINUINT8	(0x0)
#define MINUINT16       (0X0)
#define MINUINT32	(0X0ul)

#ifndef FALSE
#define FALSE           (0)
#define TRUE            (!FALSE)
#endif

#define VALID           (0)
#define NOT_VALID       (!VALID)



