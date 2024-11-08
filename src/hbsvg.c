/*
 * Harbour SVG (HBSVG) Project
 * Copyright 2014 - 2024 Rafał Jopek
 * Website: https://harbour.pl
 *
 */

#include "hbsvg.h"

/* ------------------------------------------------------------------------- */
// Garbage Collector SVG
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

static SVG *hb_svg_Param( int iParam )
{
   SVG **ppSVG = ( SVG ** ) hb_parptrGC( &s_gcSVGFuncs, iParam );

   if( ppSVG && *ppSVG )
      return *ppSVG;

   hb_errRT_BASE( EG_ARG, 3012, NULL, HB_ERR_FUNCNAME, HB_ERR_ARGS_BASEPARAMS );
   return NULL;
}

static PHB_ITEM hb_svg_ItemPut( PHB_ITEM pItem, SVG *pSVG )
{
   SVG **ppSVG = ( SVG ** ) hb_gcAllocate( sizeof( SVG * ), &s_gcSVGFuncs );

   *ppSVG = pSVG;
   return hb_itemPutPtrGC( pItem, ppSVG );
}

static void hb_svg_Return( SVG *pSVG )
{
   if( pSVG )
   {
      hb_svg_ItemPut( hb_param( -1, HB_IT_ANY ), pSVG );
   }
   else
   {
      hb_ret();
   }
}

/* ------------------------------------------------------------------------- */
// static
static void svg_line( SVG *svg, int x1, int y1, int x2, int y2, int stroke_width, unsigned int color )
{
   fprintf( svg->file, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke-width:%d; stroke:#%06x\" />\n", x1, y1, x2, y2, stroke_width, color );
}

static void svg_text( SVG *svg, int x, int y, const char *text, const char *font, int size, unsigned int color )
{
   fprintf( svg->file, "<text x=\"%d\" y=\"%d\" font-family=\"%s\" font-size=\"%d\" fill=\"#%06x\">%s</text>\n", x, y, font, size, color, text );
}

static void svg_arrow( SVG *svg, int x1, int y1, int x2, int y2, int stroke_width, unsigned int color )
{
   // Draw a line from ( x1, y1 ) to ( x2, y2 )
   svg_line( svg, x1, y1, x2, y2, stroke_width, color );

   // Calculate the angle of the line
   double angle = atan2( ( double )( y2 - y1 ), ( double )( x2 - x1 ) );

   // Length of the arrow head
   int arrow_length = 10;

   // Angles for the arrow heads
   double angle1 = angle + M_PI / 6.0;
   double angle2 = angle - M_PI / 6.0;

   // Calculate the endpoints for the arrow head
   int x3 = x2 - ( int ) ( arrow_length * cos( angle1 ) );
   int y3 = y2 - ( int ) ( arrow_length * sin( angle1 ) );

   int x4 = x2 - ( int ) ( arrow_length * cos( angle2 ) );
   int y4 = y2 - ( int ) ( arrow_length * sin( angle2 ) );

   // Draw the "head" of the arrow
   svg_line( svg, x2, y2, x3, y3, stroke_width, color );
   svg_line( svg, x2, y2, x4, y4, stroke_width, color );
}

/* ------------------------------------------------------------------------- */
// API functions
/* svg_init( <cFileName>, <nWidth>, <nHeight> ) --> <pHandle> | NIL */
HB_FUNC( SVG_INIT )
{
   SVG *svg = hb_xgrab( sizeof( SVG ) );
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

      hb_svg_Return( svg );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_set_background( <pHandle>, <nHexColor> ) --> NIL */
HB_FUNC( SVG_SET_BACKGROUND )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      unsigned long hexColor = hb_parnl( 2 );

      if( hexColor <= 0xFFFFFF )
      {
         // No alpha channel, use full opacity
         fprintf( svg->file, "<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%06lX\" fill-opacity=\"1\"/>\n", svg->width, svg->height, hexColor );
      }
      else if( hexColor <= 0xFFFFFFFF )
      {
         // Alpha channel is available
         double a = ( hexColor & 0xFF ) / 255.0;
         unsigned int color = ( hexColor >> 8 ) & 0xFFFFFF;
         fprintf( svg->file, "<rect x=\"0\" y=\"0\" width=\"%d\" height=\"%d\" fill=\"#%06X\" fill-opacity=\"%f\"/>\n", svg->width, svg->height, color, a );
      }
      else
      {
         fprintf( stderr, "Invalid hex value passed\n" );
      }
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_close( <pHandle> ) --> <lOK> */
HB_FUNC( SVG_CLOSE )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      fprintf( svg->file, "</svg>" );
      fclose( svg->file );
      hb_xfree( svg );
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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      int x1 = hb_parni( 2 );
      int y1 = hb_parni( 3 );
      int x2 = hb_parni( 4 );
      int y2 = hb_parni( 5 );
      int stroke_width = hb_parni( 6 );
      unsigned int color = hb_parni( 7 );

      svg_line( svg, x1, y1, x2, y2, stroke_width, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_polyline( <pHandle>, <aPoints>, <nPint_count>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_POLYLINE )
{
   SVG *svg = hb_svg_Param( 1 );
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
/* svg_arrow( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_ARROW )
{
   SVG *svg = hb_svg_Param( 1 );
   int x1 = hb_parni( 2 );
   int y1 = hb_parni( 3 );
   int x2 = hb_parni( 4 );
   int y2 = hb_parni( 5 );
   int stroke_width = hb_parni( 6 );
   unsigned int color = hb_parni( 7 );

   if( svg )
   {
      svg_arrow( svg, x1, y1, x2, y2, stroke_width, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_numbered_arrow( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nStroke_width>, <nStart_num>, <nEnd_num>, <nStep>, <nColor> ) --> NIL */
HB_FUNC( SVG_NUMBERED_ARROW )
{
   SVG *svg = hb_svg_Param( 1 );
   int x1 = hb_parni( 2 );
   int y1 = hb_parni( 3 );
   int x2 = hb_parni( 4 );
   int y2 = hb_parni( 5 );
   int stroke_width = hb_parni( 6 );
   int start_num = hb_parni( 7 );
   int end_num = hb_parni( 8 );
   int step = hb_parni( 9 );
   unsigned int color = hb_parni( 10 );

   if( svg )
   {
      // Drawing an arrow
      svg_arrow( svg, x1, y1, x2, y2, stroke_width, color );

      // Determining the number of labels on the arrow
      int num_labels = ( end_num - start_num ) / step + 1;

      // Determining the spacing between labels on the arrow
      float dx = ( x2 - x1 ) / ( float ) ( num_labels - 1 );
      float dy = ( y2 - y1 ) / ( float ) ( num_labels - 1 );

      // If the arrow is vertical, adjust the label positions
      int label_offset_x = 0;
      int label_offset_y = 15;
      if( x1 == x2 )
      {
         label_offset_x = -20;  // Start with a default offset
         label_offset_y = 0;
      }

      // Adding labels and tick marks
      for( int i = 0; i < num_labels; ++i )
      {
         int x = x1 + dx * i;
         int y = y1 + dy * i;
         int num = start_num + step * i;
         char label[ 10 ];
         sprintf( label, "%d", num );

         // Draw tick mark
         int tick_length = ( i % 5 == 0 ) ? 10 : 5; // Every fifth tick mark is longer
         if( x1 == x2 )
         {
            svg_line( svg, x - tick_length, y, x, y, 1, color );
         }
         else
         {
            svg_line( svg, x, y + tick_length, x, y, 1, color );
         }

         // Adjust the label offset based on the number of digits
         int num_digits = strlen( label );
         if( x1 == x2 )  // Only adjust for vertical arrows
         {
            label_offset_x = -10 * num_digits;  // Assume each digit is about 10 units wide
         }

         // Additional adjustment for values 100 or greater
         if( num >= 100 )
         {
            label_offset_x += 3;
         }

         svg_text( svg, x + label_offset_x, y + label_offset_y, label, "Arial", 12, color );
      }
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_numbered_arrow_xy( <pHandle>, <nX1>, <nY1>, <nX2>, <nY3>, <nStroke_width>, <nStart_num>, <nEnd_num>, <nStep>, nColor> ) --> NIL */
HB_FUNC( SVG_NUMBERED_ARROW_XY )
{
   SVG *svg = hb_svg_Param( 1 );
   int x1 = hb_parni( 2 );
   int y1 = hb_parni( 3 );
   int x2 = hb_parni( 4 );
   int y3 = hb_parni( 5 );
   int stroke_width = hb_parni( 6 );
   int start_num = hb_parni( 7 );
   int end_num = hb_parni( 8 );
   int step = hb_parni( 9 );
   unsigned int color = hb_parni( 10 );

   if( svg )
   {
      // Drawing horizontal arrow
      svg_arrow( svg, x1, y1, x2, y1, stroke_width, color );
      // Drawing vertical arrow
      svg_arrow( svg, x1, y1, x1, y3, stroke_width, color );

      // Determining the number of labels on the arrow
      int num_labels = ( end_num - start_num ) / step + 1;

      // Common adjustments for labels
      int label_offset_x = 0;
      int label_offset_y = 15;

      // Adding labels and tick marks for the horizontal arrow
      float dx = ( x2 - x1 ) / ( float ) ( num_labels - 1 );
      for( int i = 0; i < num_labels; ++i )
      {
         int x = x1 + dx * i;
         int y = y1;
         int num = start_num + step * i;
         char label[ 10 ];
         sprintf( label, "%d", num );

         // Draw tick mark for horizontal arrow
         int tick_length = (i % 5 == 0) ? 10 : 5; // Every fifth tick mark is longer
         svg_line(svg, x, y + tick_length, x, y, 1, color);

         svg_text( svg, x + label_offset_x, y + label_offset_y, label, "Arial", 12, color );
      }

      // Adding labels and tick marks for the vertical arrow
      label_offset_x = -20;  // Start with a default offset for vertical labels
      label_offset_y = 0;
      float dy = ( y1 - y3 ) / ( float ) ( num_labels - 1 );
      for( int i = 0; i < num_labels; ++i )
      {
         int x = x1;
         int y = y1 - dy * i;
         int num = start_num + step * i;
         char label[ 10 ];
         sprintf( label, "%d", num );

         // Draw tick mark for vertical arrow
         int tick_length = (i % 5 == 0) ? 10 : 5; // Every fifth tick mark is longer
         svg_line(svg, x - tick_length, y, x, y, 1, color);

         // Adjust the label offset based on the number of digits
         int num_digits = strlen( label );
         label_offset_x = -10 * num_digits;  // Assume each digit is about 10 units wide

         // Additional adjustment for values 100 or greater
         if( num >= 100 )
         {
            label_offset_x += 3;
         }

         if( num != 0 )  // Skip zero for the vertical arrow
         {
            svg_text( svg, x + label_offset_x, y + label_offset_y, label, "Arial", 12, color );
         }
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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
   SVG *svg = hb_svg_Param( 1 );

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
/* svg_bezier_curve( <pHandle>, <aPoints>, <nPoint_count>, <nStroke_width>, <nColor> ) --> NIL */
HB_FUNC( SVG_BEZIER_CURVE )
{
   SVG *svg = hb_svg_Param( 1 );
   PHB_ITEM pItem;

   if( svg && ( pItem = hb_param( 2, HB_IT_ARRAY ) ) != NULL )
   {
      int point_count = hb_parni( 3 );
      int stroke_width = hb_parni( 4 );
      unsigned int color = hb_parni( 5 );

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

      fprintf( svg->file, "<path d=\"M %d %d ", points[ 0 ], points[ 1 ] );

      for( int i = 2; i < point_count; i += 6 )
      {
         fprintf( svg->file, "C %d %d, %d %d, %d %d ", points[ i ], points[ i + 1 ], points[ i + 2 ], points[ i + 3 ], points[ i + 4 ], points[ i + 5 ] );
      }

      fprintf( svg->file, "\" stroke=\"#%06x\" stroke-width=\"%d\" fill=\"none\"/>\n", color, stroke_width );

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

/* svg_text( <pHandle>, <nX>, <nY>, <cText>, <cFont>, <nSize>, <nColor> ) --> NIL */
HB_FUNC( SVG_TEXT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      int x = hb_parni( 2 );
      int y = hb_parni( 3 );
      const char *text = hb_parc( 4 );
      const char *font = hb_parc( 5 );
      int size = hb_parni( 6 );
      unsigned int color = hb_parni( 7 );

      svg_text( svg, x, y, text, font, size, color );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* Linear gradient */
/* svg_linear_gradient( <pHandle>, <cId>, <nStartColor>, <nEndColor>, <nX1>, <nY1>, <nX2>, <nY2> ) --> NIL */
HB_FUNC( SVG_LINEAR_GRADIENT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      const char *id = hb_parc( 2 );
      unsigned int startColor = hb_parni( 3 );
      unsigned int endColor = hb_parni( 4 );
      float x1 = hb_parnd( 5 );
      float y1 = hb_parnd( 6 );
      float x2 = hb_parnd( 7 );
      float y2 = hb_parnd( 8 );

      fprintf( svg->file, "<defs>\n" );
      fprintf( svg->file, "<linearGradient id=\"%s\" x1=\"%f%%\" y1=\"%f%%\" x2=\"%f%%\" y2=\"%f%%\">\n", id, x1, y1, x2, y2 );
      fprintf( svg->file, "<stop offset=\"0%%\" style=\"stop-color:#%06x;stop-opacity:1\" />\n", startColor );
      fprintf( svg->file, "<stop offset=\"100%%\" style=\"stop-color:#%06x;stop-opacity:1\" />\n", endColor );
      fprintf( svg->file, "</linearGradient>\n" );
      fprintf( svg->file, "</defs>\n" );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_triangle_linear_gradient( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nX3>, <nY3>, <nStartColor>, <nEndColor> ) --> NIL */ // to be fixed
HB_FUNC( SVG_TRIANGLE_LINEAR_GRADIENT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      int x1 = hb_parni( 2 );
      int y1 = hb_parni( 3 );
      int x2 = hb_parni( 4 );
      int y2 = hb_parni( 5 );
      int x3 = hb_parni( 6 );
      int y3 = hb_parni( 7 );
      unsigned int startColor = hb_parni( 8 );
      unsigned int endColor = hb_parni( 9 );

      // Definition of a linear gradient triangle
      static int gradient_id = 0;
      fprintf( svg->file, "<defs>\n" );
      fprintf( svg->file, "  <linearGradient id=\"triangleGradient%d\" x1=\"0%%\" y1=\"0%%\" x2=\"100%%\" y2=\"0%%\">\n", gradient_id );
      fprintf( svg->file, "    <stop offset=\"0%%\" style=\"stop-color:#%06x;stop-opacity:1\" />\n", startColor );
      fprintf( svg->file, "    <stop offset=\"100%%\" style=\"stop-color:#%06x;stop-opacity:1\" />\n", endColor );
      fprintf( svg->file, "  </linearGradient>\n" );
      fprintf( svg->file, "</defs>\n" );

      // Drawing a triangle with a gradient
      fprintf( svg->file, "<polygon points=\"%d,%d %d,%d %d,%d\" fill=\"url(#triangleGradient%d)\"/>\n", x1, y1, x2, y2, x3, y3, gradient_id );

      gradient_id++; // Increment the gradient ID
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* Radial gradient */
/* svg_radial_gradient( <pHandle>, cId, nInnerColor, nOuterColor, nCx, nCy, nR ) --> NIL */
HB_FUNC( SVG_RADIAL_GRADIENT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      const char *id = hb_parc( 2 );
      unsigned int innerColor = hb_parni( 3 );
      unsigned int outerColor = hb_parni( 4 );
      float cx = hb_parnd( 5 );
      float cy = hb_parnd( 6 );
      float r = hb_parnd( 7 );

      fprintf( svg->file, "<defs>\n" );
      fprintf( svg->file, "<radialGradient id=\"%s\" cx=\"%f%%\" cy=\"%f%%\" r=\"%f%%\">\n", id, cx, cy, r );
      fprintf( svg->file, "<stop offset=\"0%%\" style=\"stop-color:#%06x;stop-opacity:1\"/>\n", innerColor );
      fprintf( svg->file, "<stop offset=\"100%%\" style=\"stop-color:#%06x;stop-opacity:1\"/>\n", outerColor );
      fprintf( svg->file, "</radialGradient>\n" );
      fprintf( svg->file, "</defs>\n" );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_triangle_radial_gradient( <pHandle>, <nX1>, <nY1>, <nX2>, <nY2>, <nX3>, <nY3>, <nStartColor>, <nEndColor> ) --> NIL */ // to be fixed
HB_FUNC( SVG_TRIANGLE_RADIAL_GRADIENT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      int x1 = hb_parni( 2 );
      int y1 = hb_parni( 3 );
      int x2 = hb_parni( 4 );
      int y2 = hb_parni( 5 );
      int x3 = hb_parni( 6 );
      int y3 = hb_parni( 7 );
      unsigned int startColor = hb_parni( 8 );
      unsigned int endColor = hb_parni( 9 );

      // Definition of a radial gradient triangle
      static int gradient_id = 0;
      fprintf( svg->file, "<defs>\n" );
      fprintf( svg->file, "  <radialGradient id=\"triangleRadialGradient%d\" cx=\"50%%\" cy=\"50%%\" r=\"50%%\">\n", gradient_id );
      fprintf( svg->file, "    <stop offset=\"0%%\" style=\"stop-color:#%06x;stop-opacity:1\" />\n", startColor );
      fprintf( svg->file, "    <stop offset=\"100%%\" style=\"stop-color:#%06x;stop-opacity:1\" />\n", endColor );
      fprintf( svg->file, "  </radialGradient>\n" );
      fprintf( svg->file, "</defs>\n" );

      // Drawing a triangle with a gradient
      fprintf( svg->file, "<polygon points=\"%d,%d %d,%d %d,%d\" fill=\"url(#triangleRadialGradient%d)\"/>\n", x1, y1, x2, y2, x3, y3, gradient_id );

      gradient_id++; // Increment the gradient ID
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_rect_gradient( <pHandle>, nX, nY, nWidth, nHeight, cGradient_id ) */
HB_FUNC( SVG_RECT_GRADIENT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      int x = hb_parni( 2 );
      int y = hb_parni( 3 );
      int width = hb_parni( 4 );
      int height = hb_parni( 5 );
      const char *gradient_id = hb_parc( 6 );

      fprintf( svg->file, "<rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"url(#%s)\"/>\n", x, y, width, height, gradient_id );
   }
   else
   {
      HB_ERR_ARGS();
   }
}

/* svg_circle_gradient( <pHandle>, nCx, nCy, nR, cGradient_id ) --> NIL */
HB_FUNC( SVG_CIRCLE_GRADIENT )
{
   SVG *svg = hb_svg_Param( 1 );

   if( svg )
   {
      int cx = hb_parni( 2 );
      int cy = hb_parni( 3 );
      int r = hb_parni( 4 );
      const char *gradient_id = hb_parc( 5 );

      fprintf( svg->file, "<circle cx=\"%d\" cy=\"%d\" r=\"%d\" fill=\"url(#%s)\"/>\n", cx, cy, r, gradient_id );
   }
   else
   {
      HB_ERR_ARGS();
   }
}
