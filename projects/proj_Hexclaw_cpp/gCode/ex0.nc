

%
(comment)
G21 G17 G90 F100
M03 S1000     
G00 X5 Y5                 ; point B
G01 X5 Y5 Z-1             ; point B
G01 X5 Y15 Z-1            ; point C
; G03 X9 Y19 Z-1 I4 J0      ; point D
G02 X4 Y14 Z-1 R4
G01 X23 Y19 Z-1           ; point E
G01 X32 Y5 Z-1            ; point F
G01 X21 Y5 Z-1            ; point G
G01 X21 Y8 Z-1            ; point H
G03 X19 Y10 Z-1 I-2 J0    ; point I
G01 X13 Y10 Z-1           ; point J
G03 X11 Y8 Z-1 I0 J-2     ; point K
G01 X11 Y5 Z-1            ; point L
G01 X5 Y5 Z-1             ; point B
G01 X5 Y5 Z0 a0 b0 y0
G02 X5 Y5 Z0 R2
G04 P100
G28 X0 Y0
M05
M30
%