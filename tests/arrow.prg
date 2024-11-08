/*
 * hbmk2 arrow.prg hbct.hbc
 */

PROCEDURE Main()

   LOCAL svg := svg_init( "arrow.svg", 400, 400 )
   LOCAL startX, startY, length
   LOCAL startAngle, endAngle, nStep, angle, radianAngle
   LOCAL endX, endY
   LOCAL red, green, blue, color

   svg_set_background( svg, 0xFFFFFF )

   // Początkowy punkt dla strzałek (lewym dolnym rogu)
   startX := 50
   startY := 350

   // Długość strzałek
   length := 300

   // Kąt początkowy i końcowy
   startAngle := 90.0
   endAngle := 18.0

   // Krok dla kąta (na przykład 10 stopni)
   nStep := 10.0

   // Rysowanie strzałek
   FOR angle := startAngle TO endAngle STEP -nStep

      radianAngle := angle * ( Pi() / 180.0 )
      endX := startX + ( length * cos( radianAngle ) )
      endY := startY - ( length * sin( radianAngle ) )

      // Zmiana koloru strzałki w zależności od kąta
      red   := 255 * sin( radianAngle )
      green := 255 * cos( radianAngle )
      blue  := 255 - red - green
      color := hb_BitShift( red, 16 ) + hb_BitShift( green, 8 ) + blue

      svg_arrow( svg, startX, startY, endX, endY, 1, color )
   NEXT

   svg_close( svg )
RETURN

