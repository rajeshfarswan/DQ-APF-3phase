/* Quadrature DQ to three phase ABC transfromation */
/* DQ to two phase Alpha-Beta */
/* Alpha-beta to three phase A-B-C */
 
;this function access integer variables Dvalue and Qvalue in main
;alters integer variables Avalue, Bvalue and Cvalue in main
;use variable PWM_max and PWM_min for limitng DQ value
;passing and return arguments are void
;calls assembly function asmINT_MULQ
;all operating vaiables are in q15 value, -0.999 <= value <= +0.999

#include "p30f6010A.inc"

.global _asmDQtoABC

_asmDQtoABC:
disi #0x3FFF

;DQ to alpha
;alpha = (D*Cosine - Q*Sine)
MOV _Dvalue, W0     ;copy D value
MOV _qCos, W1
CALL _asmINT_MPQ    ;D*Cosine = x
MOV W0,W9

MOV _Qvalue, W0     ;copy Q value
MOV _qSin, W1
CALL _asmINT_MPQ    ;Q*Sine

SUB W9,W0,W7        ;Alpha = x-y
;

;DQ to Beta
;beta =  (D*Sine + Q*Cosine);
MOV _Dvalue, W0
MOV _qSin, W1
CALL _asmINT_MPQ    ;D*Sine = x
MOV W0,W9

MOV _Qvalue, W0
MOV _qCos, W1
CALL _asmINT_MPQ    ;Q*Cosine = y

ADD W9,W0,W8        ;Beta = x+y
;

;alpha-beta to Avalue
;a = alpha
MOV W7, _Avalue     ;a = alpha 
;

;alpha-beta to Bvalue
;b = (-1/2)*alpha + (sqrt(3)/2)*beta
MOV #0x6eda, W0
MOV W8, W1
CALL _asmINT_MPQ    ;(sqrt(3)/2)*beta = x
MOV W0,W9

MOV #0x4000, W0
MOV W7,W1
CALL _asmINT_MPQ    ;0.5*alpha = y 

SUB W9,W0,W0        ;b = x-y
MOV W0, _Bvalue
;

;alpha-beta to Cvalue
;c = (-1/2)*alpha - (sqrt(3)/2)*beta
MOV #0x9126, W0
MOV W8, W1
CALL _asmINT_MPQ    ;(-sqrt(3)/2)*beta = x
MOV W0,W9

MOV #0x4000, W0
MOV W7,W1
CALL _asmINT_MPQ    ;0.5*alpha = y   

SUB W9,W0,W0
MOV W0, _Cvalue     ;c = x-y

disi #0x0000
return 
.end
