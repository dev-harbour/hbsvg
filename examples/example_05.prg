/*
 *
 */

PROCEDURE Main()

   LOCAL svg := svg_init( "example_05.svg", 500, 500 )
   LOCAL bezier_points := { 50, 250, 150, 50, 350, 450, 450, 250 }

   svg_bezier_curve( svg, bezier_points, 4, 2, 0xFF0000 )

   svg_close( svg )

RETURN 

