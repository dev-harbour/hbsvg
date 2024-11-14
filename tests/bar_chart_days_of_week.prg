/*
 *
 */

#include "hbsvg.ch"

FUNCTION random_color()
   RETURN Int( hb_Random() * 0xFFFFFF )

PROCEDURE Main()

   LOCAL svg := svg_init( "bar_chart.svg", 850, 300 )
   LOCAL days, values
   LOCAL bar_width := 80
   LOCAL bar_spacing := 20
   LOCAL x_pos := 100  // początkowa pozycja X dla pierwszego słupka
   LOCAl i, color

   svg_set_background( svg, 0xFFFFFF )

   // Rysowanie osi
   svg_arrow( svg, 50, 275, 825, 275, 1, 0x000000 )  // Oś X
   svg_arrow( svg, 50, 275, 50, 50, 1, 0x000000 )   // Oś Y

   // Dni i wartości
   days := { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" }
   values := { 10, 20, 30, 25, 15, 40, 50 }

   FOR i = 1 TO 7
      color := random_color()
      svg_filled_rect( svg, x_pos, 275 - values[ i ], bar_width, values[ i ], color )
      svg_text( svg, x_pos, 290, days[ i ], "Arial", 12, FONT_WEIGHT_NORMAL, 0x000000 ) // Tekst wyrównany do początku słupka
      x_pos += bar_width + bar_spacing  // aktualizacja pozycji X dla następnego słupka
   NEXT

   svg_close( svg )
RETURN

