/* Three phase a-b-c reference to DQ transfromation         */
/* Three phase A-B-C to Two phase Alpha-Beta transformation */
/* Two phase Alpha-Beta to quadrature D-Q transformation    */

/* this function access integer variables Avalue, Bvalue and Cvalue in main */
/* alters integer variables Dvalue and Qvalue in main */
/* passing and return arguments are void */
/* calls assembly function asmINT_MPQ */
/* all operating vaiables are in q15 value,
   -0.999 <= value <= +0.999 */

#include "p30f6010A.inc"

.global _asmABCtoDQ

_asmABCtoDQ:
disi #0x3FFF

MOV _Avalue, W7 ;copy three phase refs to temp register
MOV _Bvalue, W8 
MOV _Cvalue, W9

;abc to alpha
;alpha = (2/3)*(a - ((0.5)*b + (0.5)*c));
ADD W8,W9,W0       ;a+b = x           
MOV #0x4000, W1    
CALL _asmINT_MPQ   ;0.5*x = y 
SUB W7,W0,W0       ;a-y = z
MOV #0x5555,W1
CALL _asmINT_MPQ   ;(2/3)*z
MOV W0, W7         ;copy Alpha
;

;abc to beta
;beta =  (1/sqrt(3))*(b-c);
SUB W8,W9,W0       ;b-c = x
MOV #0x49e7,W1
CALL _asmINT_MPQ   ;(1/sqrt(3))*x
MOV W0, W8         ;copy Beta
;

;alpha-beta to D
;D = (alpha*Cosine + beta*Sine);
MOV W7,W0
MOV _qCos, W1
CALL _asmINT_MPQ   ;alpha*Cosine = x
MOV W0, W9

MOV W8,W0
MOV _qSin, W1
CALL _asmINT_MPQ   ;beta*Sine = y   

ADD W9,W0,W0
MOV W0, _Dvalue    ;copy x+y to D value 
;

;alpha-beta to Q
;Q = (-alpha*Sine + beta*Cosine);
MOV W8,W0
MOV _qCos, W1
CALL _asmINT_MPQ   ;beta*Cosine = x
MOV W0, W9

MOV W7,W0
MOV _qSin, W1      
CALL _asmINT_MPQ   ;alpha*Sine = y

SUB W9,W0,W0
MOV W0, _Qvalue    ;copy x-y to Q value
;
disi #0x0000
return 
.end
