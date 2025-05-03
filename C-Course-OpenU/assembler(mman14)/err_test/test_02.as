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

not #2047, #-2048
not sz, LABEL
not xx, ARR[2]
not #547, r4

; direct addressing

not STR, #-902
not Y, ARR
not ARR, STR[3]
not STR, r4

; direct index addressing

not ARR[4], xx
not STR[2], Y
not ARR[6], NUM[9]
not NUM[3], r0

; register
not r2, #19
not r3, LOOP
not r4, NUM[4]
not r5, r6

; clr operation

clr

; immediate addressing

clr #2047, #-2048
clr sz, LABEL
clr xx, ARR[2]
clr #547, r4

; direct addressing

clr STR, #-902
clr Y, ARR
clr ARR, STR[3]
clr STR, r4

; direct index addressing

clr ARR[4], xx
clr STR[2], Y
clr ARR[6], NUM[9]
clr NUM[3], r0

; register
clr r2, #19
clr r3, LOOP
clr r4, NUM[4]
clr r5, r6

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

inc #2047, #-2048
inc sz, LABEL
inc xx, ARR[2]
inc #547, r4

; direct addressing

inc STR, #-902
inc Y, ARR
inc ARR, STR[3]
inc STR, r4

; direct index addressing

inc ARR[4], xx
inc STR[2], Y
inc ARR[6], NUM[9]
inc NUM[3], r0

; register
inc r2, #19
inc r3, LOOP
inc r4, NUM[4]
inc r5, r6

; dec operation

dec 

; immediate addressing

dec #2047, #-2048
dec sz, LABEL
dec xx, ARR[2]
dec #547, r4

; direct addressing

dec STR, #-902
dec Y, ARR
dec ARR, STR[3]
dec STR, r4

; direct index addressing

dec ARR[4], xx
dec STR[2], Y
dec ARR[6], NUM[9]
dec NUM[3], r0

; register
dec r2, #19
dec r3, LOOP
dec r4, NUM[4]
dec r5, r6

; jmp operation

jmp 

; immediate addressing

jmp #2047, #-2048
jmp sz, LABEL
jmp xx, ARR[2]
jmp #547, r4

; direct addressing

jmp STR, #-902
jmp Y, ARR
jmp ARR, STR[3]
jmp STR, r4

; direct index addressing

jmp ARR[4], xx
jmp STR[2], Y
jmp ARR[6], NUM[9]
jmp NUM[3], r0

; register
jmp r2, #19
jmp r3, LOOP
jmp r4, NUM[4]
jmp r5, r6

;

; labels and constants defintions
LABEL: cmp r1, r2
ARR: .data -2048, 2047, 1, 2, 3, 4, 5, 0
STR: .string "abcdef"
Y: .data 10
LOOP: jmp LABEL
NUM: -1 , -2, -3, -4 , -5, -6, -7, -8, -9, -10
.define const = -1297
.define sz = 9
.define xx = -2048