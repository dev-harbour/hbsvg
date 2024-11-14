/*
 *
 */

#include "hbsvg.ch"

PROCEDURE Main()

   LOCAL hSvg, aCol
   FIELD CODE, NAME, RESIDENTS

   dbCreate( "country", { { "CODE", "C", 3, 0 }, { "NAME", "C", 30, 0 }, { "RESIDENTS", "N", 10, 0 } },, .T. )
   dbAppend(); CODE := "LTU";  NAME := "Lithuania";                 RESIDENTS :=   3369600
   dbAppend(); CODE := "USA";  NAME := "United States of America";  RESIDENTS := 305397000
   dbAppend(); CODE := "POR";  NAME := "Portugal";                  RESIDENTS :=  10617600
   dbAppend(); CODE := "POL";  NAME := "Poland";                    RESIDENTS :=  38115967
   dbAppend(); CODE := "AUS";  NAME := "Australia";                 RESIDENTS :=  21446187
   dbAppend(); CODE := "FRA";  NAME := "France";                    RESIDENTS :=  64473140
   dbAppend(); CODE := "RUS";  NAME := "Russia";                    RESIDENTS := 141900000

   hSvg := svg_init( "table.svg", 566, 793 )

   svg_text( hSvg, 50, 50, "Table of countries", "Arial", 16, FONT_WEIGHT_NORMAL, 0xFF0000 )

   aCol := { { "Code", "CODE", 60 }, { "Country", "NAME", 200 }, { "Residents", "RESIDENTS", 90 } }
   draw_table( hSvg, 50, 75, aCol )

   svg_close( hSvg )
   dbCloseAll()

RETURN

STATIC PROCEDURE draw_table( hSvg, nX, nY, aCol )

   LOCAL nI, nDX, xValue, cValue, nTextWidth

   nDX := nX

   svg_line( hSvg, nX, nY + 5, nX + 350, nY + 5, 1, 0x000000 )

   FOR nI := 1 TO Len( aCol )

      xValue := aCol[ nI, 1 ]

      nTextWidth := Len( xValue ) * 6

      svg_text( hSvg, nDX + ( aCol[ nI, 3 ] - nTextWidth ) / 2, nY, xValue, "Arial", 10, FONT_WEIGHT_NORMAL, 0x000000 )

      nDX += aCol[ nI, 3 ]

      IF nI < Len( aCol )
         svg_line( hSvg, nDX, nY - 15, nDX, nY + 200, 1, 0x000000 )
      ENDIF

   NEXT

   nY += 20
   dbGoTop()

   DO WHILE ! Eof()

      nDX := nX

      FOR nI := 1 TO Len( aCol )

         xValue := FieldGet( FieldPos( aCol[ nI, 2 ] ) )

         IF ValType( xValue ) == "N"
            cValue := hb_NToS( xValue )
         ELSE
            cValue := xValue
         ENDIF

         svg_text( hSvg, nDX + 10, nY + 10, cValue, "Arial", 10, FONT_WEIGHT_NORMAL, 0x000000 )

         nDX += aCol[ nI, 3 ]

      NEXT

      dbSkip()

      nY += 20

   ENDDO

RETURN
