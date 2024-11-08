/*
 *
 */

PROCEDURE Main()

   LOCAL x, y, i, radius := 80
   LOCAL color
   LOCAL svg

   svg := svg_init( "example_02.svg", 400, 400 )

   svg_set_background( svg, 0xFFFFFF )

   FOR i := 0 TO 44
      x = sin( i ) * radius + 200
      y = cos( i ) * radius + 200
      color = Random() % 0xFFFFFF
      svg_circle( svg, x, y, radius, 1, color )
   NEXT

   svg_close( svg )

RETURN

