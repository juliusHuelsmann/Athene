// Analyse 1) 
// finde schwarze konturen im Bildsegment. (horizontale Linie ausgehend von [x])
// 	teile bereich in 2 Farbsegmente ein (grob 100 x 30 field)

// Analyse 2) 
// Schrifterkennung (Hough voting space?) (26  + 10 = 36 * Fonts) Voting procedures ABER: Keine (bzw sehr geringe (kursiv?)) Rotationsinvarianz!



xPos = GET_X_LPARAM(lParam); 
yPos = GET_Y_LPARAM(lParam);