#ifndef HBSVG_H
#define HBSVG_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hbapi.h"
//#include "hbapifs.h"
#include "hbapierr.h"
#include "hbapiitm.h"
//#include "hbapistr.h"
//#include "hbstack.h"

typedef enum   _bool bool;

enum _bool
{
   F = 0,
   T = ( ! 0 )
};

typedef struct
{
   FILE *file;
   int width;
   int height;
} SVG;

#define HB_ERR_ARGS() ( hb_errRT_BASE_SubstR( EG_ARG, 3012, NULL, HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS ) )

#endif /* HBSVG_H */