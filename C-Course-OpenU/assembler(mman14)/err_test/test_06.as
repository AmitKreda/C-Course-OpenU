; legal data
.define a = 2
.define b = -3
.define c = -894
RRR: .data 12, -901, a, b, 16, 404
NOTDATA: not r4
VAR: .data 30

; errors
not r3 ;register number 3
hlt text
add RRR[3], c, b
.string "abcd
.string "yes"
.ext HELLO
cmp #12e+2, #-12
mov r1, %$#
prn #2048
prn #-2049
ERRSTR: .string
lea r1 r2
jmp
lea r1, 
add #1
cmp ARR[], ARR[1]
cmp ARR[, a
END hlt
DATA .data 1, 2, 3
STRR .string "string"
.entry
.extern
.define x 9
.define x=
.define =9
.data
DATA: .data
.entry a
.extern b
.entry #1
.extern r3
.define x = a
.define x = #900

.data 1, a, r2
DATA: .data b, c, RRR, -10
add #RRR[2], r2
cmp RRR[3], #VAR
.string VAR
STRR: .string mov
.define MAX 10
add sub, lea
lea .data -30, r4
jmp .string "str"
cmp RRR[VAR], #a
mov RRR[#2], r4
.define MAX_VALUE = 30
jmp , VAR
mov ,r1 r2
cmp r1, , #3
LABEL : add #1, ARR
mov ARR   [2], r1
add # b, r7
sub # -19, r0
LABEL: .define max = 10
THISLABELISWAAAAAAAAAAAAAAAAAAAAAAAAAAAAYTOOLONG: prn VAR
.define THISCONSTANTISWAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYTOOLONG = 4
.data 1, , 2
.data 1, 2,
.define twice = 4
.define twice = 5
.define same = 1
.define same =1
prn #doesntexist
cmp #nonexistent, r1
add #VAR, r4
REDEFINE: .data 1, 2, 3
REDEFINE: .string "1, 2, 3"
lea #4, r4
jmp RRR[2]
jmp NOWHERE
cmp ARR[1000], #12
