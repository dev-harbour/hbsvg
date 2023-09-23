/*
 * Copyright 2023 Rafał Jopek ( rafaljopek at hotmail com )
 */

#include "hbsvg.h"

static HB_GARBAGE_FUNC( hb_svg_destructor )
{
   SVG **ppSVG = ( SVG ** ) Cargo;

   if( *ppSVG )
   {
      *ppSVG = NULL;
   }
}

static const HB_GC_FUNCS s_gcSVGFuncs =
{
   hb_svg_destructor,
   hb_gcDummyMark
};

SVG *hb_svgParam( int iParam )
{
   SVG **ppSVG = ( SVG ** ) hb_parptrGC( &s_gcSVGFuncs, iParam );

   if( ppSVG && *ppSVG )
      return *ppSVG;

   hb_errRT_BASE( EG_ARG, 3012, NULL, HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS );
   return NULL;
}

PHB_ITEM hb_svgItemPut( PHB_ITEM pItem, SVG *pSVG )
{
   SVG **ppSVG = ( SVG ** ) hb_gcAllocate( sizeof( SVG * ), &s_gcSVGFuncs );

   *ppSVG = pSVG;
   return hb_itemPutPtrGC( pItem, ppSVG );
}

void hb_svgReturn( SVG *pSVG )
{
   if( pSVG )
   {
      hb_svgItemPut( hb_param( -1, HB_IT_ANY ), pSVG );
   }
   else
   {
      hb_ret();
   }
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* svg_init( <cFileName>, <nWidth>, <nHeight> ) --> <pHandle> | NIL */
HB_FUNC( SVG_INIT )
{
   SVG *svg = malloc( sizeof( SVG ) );
   if( svg == NULL )
   {
      fprintf( stderr, "Error: Could not allocate memory for SVG structure.\n" );
      hb_ret(); // Return NIL to indicate failure
   }

   const char *filename = hb_parc( 1 );

   if( filename )
   {
      svg->file = fopen( filename, "w" );
      if( svg->file == NULL )
      {
         fprintf( stderr, "Error: Could not open file '%s' for writing.\n", filename );
         free( svg ); // Don't forget to free the previously allocated memory
         hb_ret();    // Return NIL to indicate failure
      }

      svg->width = hb_parni( 2 );
      svg->height = hb_parni( 3 );

      fprintf( svg->file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
      fprintf( svg->file, "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" " );
      fprintf( svg->file, "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n" );
      fprintf( svg->file, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n", svg->width, svg->height, svg->width, svg->height );

      hb_svgReturn( svg );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_set_background( <pHandle>, <nColor> ) --> NIL */
HB_FUNC( SVG_SET_BACKGROUND )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      unsigned int color = hb_parni( 2 );

      fprintf( svg->file, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#%06x\"/>\n", 0, 0, svg->width, svg->height, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_close( <pHandle> ) --> <lOK> */
HB_FUNC( SVG_CLOSE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      fprintf( svg->file, "</svg>" );
      fclose( svg->file );
      free( svg );
      hb_retl( HB_TRUE );
   }
   else 
   {
      fprintf( stderr, "Error: svg_close called with NULL SVG pointer.\n" );
      HB_ERR_ARGS();
   }
}

/* svg_rect( <pHandle>, <nX>, <nY>, <nWidth>, <nHeight>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_RECT )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int x = hb_parni( 2 );
      int y = hb_parni( 3 );
      int width = hb_parni( 4 );
      int height = hb_parni( 5 );
      int stroke_width = hb_parni( 6 );
      unsigned int color = hb_parni( 7 );

      fprintf( svg->file, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" stroke-width=\"%d\" stroke=\"#%06x\" fill=\"none\"/>\n", x, y, width, height, stroke_width, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_filled_rect( <pHandle>, <nX>, <nY>, <nWidth>, <nHeight>, <nColor> ) --> NIL */
HB_FUNC( SVG_FILLED_RECT ) 
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int x = hb_parni( 2 );
      int y = hb_parni( 3 );
      int width = hb_parni( 4 );
      int height = hb_parni( 5 );
      unsigned int color = hb_parni( 6 );

      fprintf( svg->file, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"#%06x\"/>\n", x, y, width, height, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_triangle( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nX3>, <nY3>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_TRIANGLE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int x1 = hb_parni( 2 );
      int y1 = hb_parni( 3 );
      int x2 = hb_parni( 4 );
      int y2 = hb_parni( 5 );
      int x3 = hb_parni( 6 );
      int y3 = hb_parni( 7 );
      int stroke_width = hb_parni( 8 );
      unsigned int color = hb_parni( 9 );

      fprintf( svg->file, "<polygon points=\"%d,%d %d,%d %d,%d\" stroke-width=\"%d\" stroke=\"#%06x\" fill=\"none\"/>\n", x1, y1, x2, y2, x3, y3, stroke_width, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}
/* svg_filled_triangle( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nX3>, <nY3>, <nColor> ) --> NIL */
HB_FUNC( SVG_FILLED_TRIANGLE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int x1 = hb_parni( 2 );
      int y1 = hb_parni( 3 );
      int x2 = hb_parni( 4 );
      int y2 = hb_parni( 5 );
      int x3 = hb_parni( 6 );
      int y3 = hb_parni( 7 );
      unsigned int color = hb_parni( 8 );

      fprintf( svg->file, "<polygon points=\"%d,%d %d,%d %d,%d\" fill=\"#%06x\"/>\n", x1, y1, x2, y2, x3, y3, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_circle( <pHandle>, <nCx>, <nCy>, <nR>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_CIRCLE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int cx = hb_parni( 2 );
      int cy = hb_parni( 3 );
      int r = hb_parni( 4 );
      int stroke_width = hb_parni( 5 );
      unsigned int color = hb_parni( 6 );

      fprintf( svg->file, "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" stroke-width=\"%d\" stroke=\"#%06x\" fill=\"none\"/>\n", cx, cy, r, stroke_width, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_filled_circle( <pHandle>, <nCx>, <nCy>, <nR>, <nColor> ) --> NIL */
HB_FUNC( SVG_FILLED_CIRCLE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int cx = hb_parni( 2 );
      int cy = hb_parni( 3 );
      int r = hb_parni( 4 );
      unsigned int color = hb_parni( 5 );

      fprintf( svg->file, "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"#%06x\"/>\n", cx, cy, r, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_line( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_LINE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int x1 = hb_parni( 2 );
      int y1 = hb_parni( 3 );
      int x2 = hb_parni( 4 );
      int y2 = hb_parni( 5 );
      int stroke_width = hb_parni( 6 );
      unsigned int color = hb_parni( 7 );

      fprintf( svg->file, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke-width:%d; stroke:#%06x\" />\n", x1, y1, x2, y2, stroke_width, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_polyline( <pHandle>, <aPoints>, <nPint_count>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_POLYLINE )
{
   SVG *svg = hb_svgParam( 1 );
   PHB_ITEM pItem;

   if( svg && ( pItem = hb_param( 2, HB_IT_ARRAY ) ) != NULL )
   {
      int point_count = hb_parni( 3 );
      int stroke_width = hb_parni( 4 );
      unsigned int color = hb_parni( 5 );

      fprintf( svg->file, "<polyline points=\"" );

      point_count = ( int ) hb_arrayLen( pItem );
      int *points = NULL;

      if( point_count )
      {
         points = ( int * ) hb_xgrab( point_count * sizeof( int ) );
      }

      // Retrieving elements from the Harbour array into the points array
      for( int i = 0; i < point_count; ++i )
      {
         points[ i ] = hb_arrayGetNI( pItem, ( HB_SIZE ) i + 1 );
      }

      // Retrieving elements from the Harbour array into the points array
      for( int i = 0; i < point_count; i += 2 )
      {
         fprintf( svg->file, "%d,%d ", points[ i ], points[ i + 1 ] );
      }

      fprintf( svg->file, "\" stroke-width=\"%d\" stroke=\"#%06x\" fill=\"none\"/>\n", stroke_width, color );

      if( points )
      {
         hb_xfree( points );
      }
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_hexagon( <pHandle>, <nHx>, <nHy>, <nR>, <nStroke_width>, <lType>, <nColor> ) --> NIL */
HB_FUNC( SVG_HEXAGON )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int hx = hb_parni( 2 );
      int hy = hb_parni( 3 );
      int r = hb_parni( 4 );
      int stroke_width = hb_parni( 5 );
      bool type = hb_parl( 6 );
      unsigned int color = hb_parni( 7 );

      double a = 2 * M_PI / 6;
      double angle_offset = ( type == 0 ? M_PI_2 : M_PI / 3 ); // Decides the orientation
      double x1 = hx + r * cos( a * 5 + angle_offset );
      double y1 = hy + r * sin( a * 5 + angle_offset );
  
      fprintf( svg->file, "<polygon points=\"%.2lf,%.2lf ", x1, y1 );
  
      for( int i = 0; i < 6; ++i )
      {
         double x = hx + r * cos( a * i + angle_offset );
         double y = hy + r * sin( a * i + angle_offset );
         fprintf( svg->file, "%.2lf,%.2lf ", x, y );
      }

      fprintf( svg->file, "\" stroke-width=\"%d\" stroke=\"#%06x\"", stroke_width, color );
      fprintf( svg->file, " fill=\"none\"/>\n" );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_filled_hexagon( <pHandle>, <nHx>, <nHy>, <nR>, <lType>, <nColor> ) --> NIL */
HB_FUNC( SVG_FILLED_HEXAGON )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int hx = hb_parni( 2 );
      int hy = hb_parni( 3 );
      int r = hb_parni( 4 );
      bool type = hb_parl( 5 );
      unsigned int color = hb_parni( 6 );

      double a = 2 * M_PI / 6;
      double angle_offset = ( type == 0 ? M_PI_2 : M_PI / 3 ); // Decides the orientation
      double x1 = hx + r * cos( a * 5 + angle_offset );
      double y1 = hy + r * sin( a * 5 + angle_offset );
  
      fprintf( svg->file, "<polygon points=\"%.2lf,%.2lf ", x1, y1 );
  
      for( int i = 0; i < 6; ++i )
      {
         double x = hx + r * cos( a * i + angle_offset );
         double y = hy + r * sin( a * i + angle_offset );
         fprintf( svg->file, "%.2lf,%.2lf ", x, y );
      }

      fprintf( svg->file, "\" stroke=\"#%06x\" stroke-width=\"1\"", color );
      fprintf( svg->file, " fill=\"#%06x\"/>\n", color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_ellipse( <pHandle>, <nCx>, <nCy>, <nRx>, <nRy>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_ELLIPSE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int cx = hb_parni( 2 );
      int cy = hb_parni( 3 );
      int rx = hb_parni( 4 );
      int ry = hb_parni( 5 );
      int stroke_width = hb_parni( 6 );
      unsigned int color = hb_parni( 7 );

      fprintf( svg->file, "<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" stroke=\"#%06x\" stroke-width=\"%d\" fill=\"none\"/>\n", cx, cy, rx, ry, color, stroke_width );
   }
   else
   {
      HB_ERR_ARGS();
   }   
}

/* svg_filled_ellipse( <pHandle>, <nCx>, <nCy>, <nRx>, <nRy>, <nColor> ) --> NIL */
HB_FUNC( SVG_FILLED_ELLIPSE )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int cx = hb_parni( 2 );
      int cy = hb_parni( 3 );
      int rx = hb_parni( 4 );
      int ry = hb_parni( 5 );
      unsigned int color = hb_parni( 6 );

      fprintf( svg->file, "<ellipse cx=\"%d\" cy=\"%d\" rx=\"%d\" ry=\"%d\" fill=\"#%06x\"/>\n", cx, cy, rx, ry, color );
   }
   else
   {
      HB_ERR_ARGS();
   }   
}

/* svg_text( <pHandle>, <nX>, <nY>, <cText>, <cFont>, <nSize>, <nColor> ) --> NIL */
HB_FUNC( SVG_TEXT )
{
   SVG *svg = hb_svgParam( 1 );

   if( svg )
   {
      int x = hb_parni( 2 );
      int y = hb_parni( 3 );
      const char *text = hb_parc( 4 );
      const char *font = hb_parc( 5 );
      int size = hb_parni( 6 );
      unsigned int color = hb_parni( 7 );

      fprintf( svg->file, "<text x=\"%d\" y=\"%d\" font-family=\"%s\" font-size=\"%d\" fill=\"#%06x\">%s</text>\n", x, y, font, size, color, text );
   }
   else
   {
      HB_ERR_ARGS();
   }   
}
