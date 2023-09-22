/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#ifndef HBSVG_H
#define HBSVG_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hbapi.h"
#include "hbapifs.h"
#include "hbapierr.h"
#include "hbapiitm.h"
#include "hbapistr.h"
#include "hbstack.h"

typedef enum bool
{
   F = 0,
   T = ( ! 0 )
} bool;

typedef struct
{
   FILE *file;
   int width;
   int height;
} SVG;

HB_EXTERN_BEGIN

extern HB_EXPORT SVG     *hb_svgParam( int iParam );
extern HB_EXPORT PHB_ITEM hb_svgItemPut( PHB_ITEM pItem, SVG *pSVG );
extern HB_EXPORT void     hb_svgReturn( SVG *pSVG );

#define HB_ERR_ARGS() ( hb_errRT_BASE_SubstR( EG_ARG, 3012, NULL, HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS ) )

HB_EXTERN_END

#endif /* HBSVG_H */