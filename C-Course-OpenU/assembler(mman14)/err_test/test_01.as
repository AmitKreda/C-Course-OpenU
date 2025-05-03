; mov operation

mov

; immediate addressing

mov #2047, #-2048
mov sz, LABEL
mov xx, ARR[2]
mov #547, r4

; direct addressing

mov STR, #-902
mov Y, ARR
mov ARR, STR[3]
mov STR, r4

; direct index addressing

mov ARR[4], xx
mov STR[2], Y
mov ARR[6], NUM[9]
mov NUM[3], r0

; register
mov r2, #19
mov r3, LOOP
mov r4, NUM[4]
mov r5, r6

; cmp operation

cmp

; immediate addressing

cmp #2047, #-2048
cmp sz, LABEL
cmp xx, ARR[2]
cmp #547, r4

; direct addressing

cmp STR, #-902
cmp Y, ARR
cmp ARR, STR[3]
cmp STR, r4

; direct index addressing

cmp ARR[4], xx
cmp STR[2], Y
cmp ARR[6], NUM[9]
cmp NUM[3], r0

; register
cmp r2, #19
cmp r3, LOOP
cmp r4, NUM[4]
cmp r5, r6

; add operation

add 

; immediate addressing

add #2047, #-2048
add sz, LABEL
add xx, ARR[2]
add #547, r4

; direct addressing

add STR, #-902
add Y, ARR
add ARR, STR[3]
add STR, r4

; direct index addressing

add ARR[4], xx
add STR[2], Y
add ARR[6], NUM[9]
add NUM[3], r0

; register
add r2, #19
add r3, LOOP
add r4, NUM[4]
add r5, r6

; sub operation

sub

; immediate addressing

sub #2047, #-2048
sub sz, LABEL
sub xx, ARR[2]
sub #547, r4

; direct addressing

sub STR, #-902
sub Y, ARR
sub ARR, STR[3]
sub STR, r4

; direct index addressing

sub ARR[4], xx
sub STR[2], Y
sub ARR[6], NUM[9]
sub NUM[3], r0

; register
sub r2, #19
sub r3, LOOP
sub r4, NUM[4]
sub r5, r6

; not operation

not

; immediate addressing

not #547

; direct addressing

not STR

; direct index addressing

not NUM[3]

; register

not r5

; clr operation

clr

; immediate addressing

clr #547

; direct addressing

clr STR

; direct index addressing

clr NUM[3]

; register

clr r5

; lea operation

lea 

; immediate addressing

lea #2047, #-2048
lea sz, LABEL
lea xx, ARR[2]
lea #547, r4

; direct addressing

lea STR, #-902
lea Y, ARR
lea ARR, STR[3]
lea STR, r4

; direct index addressing

lea ARR[4], xx
lea STR[2], Y
lea ARR[6], NUM[9]
lea NUM[3], r0

; register
lea r2, #19
lea r3, LOOP
lea r4, NUM[4]
lea r5, r6

; inc operation

inc 

; immediate addressing

inc #547

; direct addressing

inc STR

; direct index addressing

inc NUM[3]

; register

inc r5

; dec operation

dec 

; immediate addressing

dec #547

; direct addressing

dec STR

; direct index addressing

dec NUM[3]

; register
dec r5

; jmp operation

jmp 

; immediate addressing

jmp #547

; direct addressing

jmp STR

; direct index addressing

jmp NUM[3]

; register

jmp r5

; bne operation

bne

; immediate addressing

bne #547

; direct addressing

bne STR

; direct index addressing

bne NUM[3]

; register

bne r5

; red operation

red

; immediate addressing

red #547

; direct addressing

red STR

; direct index addressing

red NUM[3]

; register

red r5

; prn operation

prn

; immediate addressing

prn #547

; direct addressing

prn STR

; direct index addressing

prn NUM[3]

; register

prn r5

;jsr operation

jsr

; immediate addressing

jsr #547

; direct addressing

jsr STR

; direct index addressing

jsr NUM[3]

; register

jsr r5

;rts operation

rts

;hlt operation

hlt

; labels and constants defintions
LABEL: cmp r1, r2
ARR: .data -2048, 2047, 1, 2, 3, 4, 5, 0
STR: .string "abcdef"
Y: .data 10
LOOP: jmp LABEL
NUM: .data -1 , -2, -3, -4 , -5, -6, -7, -8, -9, -10
.define const = -1297
.define sz = 9
.define xx = -2048