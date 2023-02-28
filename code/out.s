.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
jr $ra
write:
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
move $v0, $0
jr $ra
dist:
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
lw $18, 16($fp)
lw $23, 8($fp)
sub $17, $18, $23
sw $17, -4($fp)
addi $sp, $fp, -4
lw $8, 16($fp)
lw $23, 8($fp)
sub $11, $8, $23
sw $11, -8($fp)
addi $sp, $fp, -8
lw $11, -4($fp)
lw $22, -8($fp)
mul $19, $11, $22
sw $19, -12($fp)
addi $sp, $fp, -12
lw $10, 20($fp)
lw $13, 12($fp)
sub $22, $10, $13
sw $22, -16($fp)
addi $sp, $fp, -16
lw $11, 20($fp)
lw $10, 12($fp)
sub $20, $11, $10
sw $20, -20($fp)
addi $sp, $fp, -20
lw $13, -16($fp)
lw $19, -20($fp)
mul $21, $13, $19
sw $21, -24($fp)
addi $sp, $fp, -24
lw $14, -12($fp)
lw $22, -24($fp)
add $9, $14, $22
sw $9, -28($fp)
addi $sp, $fp, -28
lw $16, -28($fp)
move $v0, $16
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
check:
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
lw $10, 20($fp)
sw $10, -4($sp)
lw $13, 16($fp)
sw $13, -8($sp)
lw $22, 12($fp)
sw $22, -12($sp)
lw $16, 8($fp)
sw $16, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $22, $v0
sw $22, -20($fp)
addi $sp, $fp, -20
lw $22, -20($fp)
li $19, 0
bgt $22, $19, L1
j L6
L1:
lw $8, 20($fp)
sw $8, -4($sp)
lw $23, 16($fp)
sw $23, -8($sp)
lw $18, 12($fp)
sw $18, -12($sp)
lw $14, 8($fp)
sw $14, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $11, $v0
sw $11, -40($fp)
addi $sp, $fp, -40
lw $18, 28($fp)
sw $18, -4($sp)
lw $12, 24($fp)
sw $12, -8($sp)
lw $22, 20($fp)
sw $22, -12($sp)
lw $11, 16($fp)
sw $11, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $10, $v0
sw $10, -60($fp)
addi $sp, $fp, -60
lw $11, -40($fp)
lw $10, -60($fp)
beq $11, $10, L2
j L6
L2:
lw $14, 28($fp)
sw $14, -4($sp)
lw $10, 24($fp)
sw $10, -8($sp)
lw $11, 20($fp)
sw $11, -12($sp)
lw $21, 16($fp)
sw $21, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $17, $v0
sw $17, -80($fp)
addi $sp, $fp, -80
lw $10, 36($fp)
sw $10, -4($sp)
lw $19, 32($fp)
sw $19, -8($sp)
lw $17, 28($fp)
sw $17, -12($sp)
lw $12, 24($fp)
sw $12, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $14, $v0
sw $14, -100($fp)
addi $sp, $fp, -100
lw $17, -80($fp)
lw $14, -100($fp)
beq $17, $14, L3
j L6
L3:
lw $8, 36($fp)
sw $8, -4($sp)
lw $24, 32($fp)
sw $24, -8($sp)
lw $18, 28($fp)
sw $18, -12($sp)
lw $13, 24($fp)
sw $13, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $12, $v0
sw $12, -120($fp)
addi $sp, $fp, -120
lw $11, 12($fp)
sw $11, -4($sp)
lw $10, 8($fp)
sw $10, -8($sp)
lw $9, 36($fp)
sw $9, -12($sp)
lw $18, 32($fp)
sw $18, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $10, $v0
sw $10, -140($fp)
addi $sp, $fp, -140
lw $12, -120($fp)
lw $10, -140($fp)
beq $12, $10, L4
j L6
L4:
lw $22, 28($fp)
sw $22, -4($sp)
lw $19, 24($fp)
sw $19, -8($sp)
lw $21, 12($fp)
sw $21, -12($sp)
lw $11, 8($fp)
sw $11, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $18, $v0
sw $18, -160($fp)
addi $sp, $fp, -160
lw $22, 36($fp)
sw $22, -4($sp)
lw $12, 32($fp)
sw $12, -8($sp)
lw $23, 20($fp)
sw $23, -12($sp)
lw $15, 16($fp)
sw $15, -16($sp)
addi $sp, $sp, -16
addi $sp, $sp, -4
sw $ra, 0($sp)
jal dist
lw $ra, 0($sp)
addi $sp, $sp, 4
move $14, $v0
sw $14, -180($fp)
addi $sp, $fp, -180
lw $18, -160($fp)
lw $14, -180($fp)
beq $18, $14, L5
j L6
L5:
li $12, 1
sw $12, -184($fp)
addi $sp, $fp, -184
j L7
L6:
li $10, 0
sw $10, -184($fp)
L7:
lw $13, -184($fp)
move $v0, $13
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
main:
addi $sp, $sp, -4
sw $fp, 0($sp)
move $fp, $sp
addi $sp, $fp, -8
addi $sp, $fp, -16
addi $sp, $fp, -24
addi $sp, $fp, -32
addi $22, $fp, -8
sw $22, -36($fp)
addi $sp, $fp, -36
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $15, -40($fp)
addi $sp, $fp, -40
move $15, $v0
sw $15, -40($fp)
lw $23, -40($fp)
lw $25, -36($fp)
sw $23, 0($25)
addi $10, $fp, -8
sw $10, -44($fp)
addi $sp, $fp, -44
li $18, 1
li $8, 4
mul $20, $18, $8
sw $20, -48($fp)
addi $sp, $fp, -48
lw $12, -44($fp)
lw $22, -48($fp)
add $17, $12, $22
sw $17, -52($fp)
addi $sp, $fp, -52
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $24, -56($fp)
addi $sp, $fp, -56
move $24, $v0
sw $24, -56($fp)
lw $15, -56($fp)
lw $25, -52($fp)
sw $15, 0($25)
addi $11, $fp, -16
sw $11, -60($fp)
addi $sp, $fp, -60
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $14, -64($fp)
addi $sp, $fp, -64
move $14, $v0
sw $14, -64($fp)
lw $22, -64($fp)
lw $25, -60($fp)
sw $22, 0($25)
addi $19, $fp, -16
sw $19, -68($fp)
addi $sp, $fp, -68
li $18, 1
li $9, 4
mul $8, $18, $9
sw $8, -72($fp)
addi $sp, $fp, -72
lw $16, -68($fp)
lw $15, -72($fp)
add $23, $16, $15
sw $23, -76($fp)
addi $sp, $fp, -76
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $8, -80($fp)
addi $sp, $fp, -80
move $8, $v0
sw $8, -80($fp)
lw $10, -80($fp)
lw $25, -76($fp)
sw $10, 0($25)
addi $18, $fp, -24
sw $18, -84($fp)
addi $sp, $fp, -84
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $10, -88($fp)
addi $sp, $fp, -88
move $10, $v0
sw $10, -88($fp)
lw $17, -88($fp)
lw $25, -84($fp)
sw $17, 0($25)
addi $17, $fp, -24
sw $17, -92($fp)
addi $sp, $fp, -92
li $14, 1
li $19, 4
mul $11, $14, $19
sw $11, -96($fp)
addi $sp, $fp, -96
lw $22, -92($fp)
lw $23, -96($fp)
add $10, $22, $23
sw $10, -100($fp)
addi $sp, $fp, -100
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $11, -104($fp)
addi $sp, $fp, -104
move $11, $v0
sw $11, -104($fp)
lw $9, -104($fp)
lw $25, -100($fp)
sw $9, 0($25)
addi $13, $fp, -32
sw $13, -108($fp)
addi $sp, $fp, -108
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $21, -112($fp)
addi $sp, $fp, -112
move $21, $v0
sw $21, -112($fp)
lw $10, -112($fp)
lw $25, -108($fp)
sw $10, 0($25)
addi $19, $fp, -32
sw $19, -116($fp)
addi $sp, $fp, -116
li $18, 1
li $22, 4
mul $21, $18, $22
sw $21, -120($fp)
addi $sp, $fp, -120
lw $19, -116($fp)
lw $13, -120($fp)
add $12, $19, $13
sw $12, -124($fp)
addi $sp, $fp, -124
addi $sp, $sp, -4
sw $ra, 0($sp)
jal read
lw $ra, 0($sp)
addi $sp, $sp, 4
sw $17, -128($fp)
addi $sp, $fp, -128
move $17, $v0
sw $17, -128($fp)
lw $14, -128($fp)
lw $25, -124($fp)
sw $14, 0($25)
addi $14, $fp, -32
sw $14, -132($fp)
addi $sp, $fp, -132
li $23, 1
li $8, 4
mul $22, $23, $8
sw $22, -136($fp)
addi $sp, $fp, -136
lw $23, -132($fp)
lw $8, -136($fp)
add $19, $23, $8
sw $19, -140($fp)
addi $sp, $fp, -140
addi $8, $fp, -32
sw $8, -144($fp)
addi $sp, $fp, -144
addi $19, $fp, -24
sw $19, -148($fp)
addi $sp, $fp, -148
li $24, 1
li $23, 4
mul $17, $24, $23
sw $17, -152($fp)
addi $sp, $fp, -152
lw $10, -148($fp)
lw $9, -152($fp)
add $24, $10, $9
sw $24, -156($fp)
addi $sp, $fp, -156
addi $19, $fp, -24
sw $19, -160($fp)
addi $sp, $fp, -160
addi $14, $fp, -16
sw $14, -164($fp)
addi $sp, $fp, -164
li $20, 1
li $24, 4
mul $8, $20, $24
sw $8, -168($fp)
addi $sp, $fp, -168
lw $15, -164($fp)
lw $8, -168($fp)
add $22, $15, $8
sw $22, -172($fp)
addi $sp, $fp, -172
addi $19, $fp, -16
sw $19, -176($fp)
addi $sp, $fp, -176
addi $10, $fp, -8
sw $10, -180($fp)
addi $sp, $fp, -180
li $10, 1
li $23, 4
mul $20, $10, $23
sw $20, -184($fp)
addi $sp, $fp, -184
lw $22, -180($fp)
lw $18, -184($fp)
add $9, $22, $18
sw $9, -188($fp)
addi $sp, $fp, -188
addi $12, $fp, -8
sw $12, -192($fp)
addi $sp, $fp, -192
lw $24, -140($fp)
lw $24, 0($24)
sw $24, -4($sp)
lw $17, -144($fp)
lw $17, 0($17)
sw $17, -8($sp)
lw $22, -156($fp)
lw $22, 0($22)
sw $22, -12($sp)
lw $18, -160($fp)
lw $18, 0($18)
sw $18, -16($sp)
lw $19, -172($fp)
lw $19, 0($19)
sw $19, -20($sp)
lw $23, -176($fp)
lw $23, 0($23)
sw $23, -24($sp)
lw $12, -188($fp)
lw $12, 0($12)
sw $12, -28($sp)
lw $21, -192($fp)
lw $21, 0($21)
sw $21, -32($sp)
addi $sp, $sp, -32
addi $sp, $sp, -4
sw $ra, 0($sp)
jal check
lw $ra, 0($sp)
addi $sp, $sp, 4
move $20, $v0
sw $20, -228($fp)
addi $sp, $fp, -228
lw $12, -228($fp)
move $13, $12
sw $13, -232($fp)
addi $sp, $fp, -232
addi $23, $fp, -32
sw $23, -236($fp)
addi $sp, $fp, -236
li $10, 1
li $24, 4
mul $22, $10, $24
sw $22, -240($fp)
addi $sp, $fp, -240
lw $10, -236($fp)
lw $22, -240($fp)
add $23, $10, $22
sw $23, -244($fp)
addi $sp, $fp, -244
addi $15, $fp, -32
sw $15, -248($fp)
addi $sp, $fp, -248
addi $16, $fp, -16
sw $16, -252($fp)
addi $sp, $fp, -252
li $8, 1
li $9, 4
mul $22, $8, $9
sw $22, -256($fp)
addi $sp, $fp, -256
lw $11, -252($fp)
lw $23, -256($fp)
add $16, $11, $23
sw $16, -260($fp)
addi $sp, $fp, -260
addi $21, $fp, -16
sw $21, -264($fp)
addi $sp, $fp, -264
addi $18, $fp, -24
sw $18, -268($fp)
addi $sp, $fp, -268
li $23, 1
li $13, 4
mul $18, $23, $13
sw $18, -272($fp)
addi $sp, $fp, -272
lw $21, -268($fp)
lw $14, -272($fp)
add $12, $21, $14
sw $12, -276($fp)
addi $sp, $fp, -276
addi $10, $fp, -24
sw $10, -280($fp)
addi $sp, $fp, -280
addi $10, $fp, -8
sw $10, -284($fp)
addi $sp, $fp, -284
li $23, 1
li $14, 4
mul $15, $23, $14
sw $15, -288($fp)
addi $sp, $fp, -288
lw $9, -284($fp)
lw $19, -288($fp)
add $10, $9, $19
sw $10, -292($fp)
addi $sp, $fp, -292
addi $24, $fp, -8
sw $24, -296($fp)
addi $sp, $fp, -296
lw $12, -244($fp)
lw $12, 0($12)
sw $12, -4($sp)
lw $17, -248($fp)
lw $17, 0($17)
sw $17, -8($sp)
lw $13, -260($fp)
lw $13, 0($13)
sw $13, -12($sp)
lw $23, -264($fp)
lw $23, 0($23)
sw $23, -16($sp)
lw $15, -276($fp)
lw $15, 0($15)
sw $15, -20($sp)
lw $19, -280($fp)
lw $19, 0($19)
sw $19, -24($sp)
lw $21, -292($fp)
lw $21, 0($21)
sw $21, -28($sp)
lw $14, -296($fp)
lw $14, 0($14)
sw $14, -32($sp)
addi $sp, $sp, -32
addi $sp, $sp, -4
sw $ra, 0($sp)
jal check
lw $ra, 0($sp)
addi $sp, $sp, 4
move $13, $v0
sw $13, -332($fp)
addi $sp, $fp, -332
lw $14, -332($fp)
move $20, $14
sw $20, -336($fp)
addi $sp, $fp, -336
addi $12, $fp, -24
sw $12, -340($fp)
addi $sp, $fp, -340
li $19, 1
li $10, 4
mul $13, $19, $10
sw $13, -344($fp)
addi $sp, $fp, -344
lw $17, -340($fp)
lw $23, -344($fp)
add $24, $17, $23
sw $24, -348($fp)
addi $sp, $fp, -348
addi $21, $fp, -24
sw $21, -352($fp)
addi $sp, $fp, -352
addi $13, $fp, -32
sw $13, -356($fp)
addi $sp, $fp, -356
li $19, 1
li $23, 4
mul $15, $19, $23
sw $15, -360($fp)
addi $sp, $fp, -360
lw $17, -356($fp)
lw $12, -360($fp)
add $13, $17, $12
sw $13, -364($fp)
addi $sp, $fp, -364
addi $10, $fp, -32
sw $10, -368($fp)
addi $sp, $fp, -368
addi $14, $fp, -16
sw $14, -372($fp)
addi $sp, $fp, -372
li $15, 1
li $9, 4
mul $10, $15, $9
sw $10, -376($fp)
addi $sp, $fp, -376
lw $24, -372($fp)
lw $22, -376($fp)
add $8, $24, $22
sw $8, -380($fp)
addi $sp, $fp, -380
addi $22, $fp, -16
sw $22, -384($fp)
addi $sp, $fp, -384
addi $16, $fp, -8
sw $16, -388($fp)
addi $sp, $fp, -388
li $12, 1
li $20, 4
mul $10, $12, $20
sw $10, -392($fp)
addi $sp, $fp, -392
lw $18, -388($fp)
lw $15, -392($fp)
add $16, $18, $15
sw $16, -396($fp)
addi $sp, $fp, -396
addi $13, $fp, -8
sw $13, -400($fp)
addi $sp, $fp, -400
lw $17, -348($fp)
lw $17, 0($17)
sw $17, -4($sp)
lw $9, -352($fp)
lw $9, 0($9)
sw $9, -8($sp)
lw $18, -364($fp)
lw $18, 0($18)
sw $18, -12($sp)
lw $24, -368($fp)
lw $24, 0($24)
sw $24, -16($sp)
lw $8, -380($fp)
lw $8, 0($8)
sw $8, -20($sp)
lw $23, -384($fp)
lw $23, 0($23)
sw $23, -24($sp)
lw $20, -396($fp)
lw $20, 0($20)
sw $20, -28($sp)
lw $12, -400($fp)
lw $12, 0($12)
sw $12, -32($sp)
addi $sp, $sp, -32
addi $sp, $sp, -4
sw $ra, 0($sp)
jal check
lw $ra, 0($sp)
addi $sp, $sp, 4
move $20, $v0
sw $20, -436($fp)
addi $sp, $fp, -436
lw $23, -436($fp)
move $17, $23
sw $17, -440($fp)
addi $sp, $fp, -440
lw $13, -232($fp)
li $22, 0
bne $13, $22, L10
j L8
L8:
lw $20, -336($fp)
li $14, 0
bne $20, $14, L10
j L9
L9:
lw $17, -440($fp)
li $13, 0
bne $17, $13, L10
j L11
L10:
li $24, 1
sw $24, -444($fp)
addi $sp, $fp, -444
j L12
L11:
li $16, 0
sw $16, -444($fp)
L12:
lw $21, -444($fp)
move $a0, $21
addi $sp, $sp, -4
sw $ra, 0($sp)
jal write
lw $ra, 0($sp)
addi $sp, $sp, 4
li $12, 0
move $v0, $12
move $sp, $fp
lw $fp, 0($sp)
addi $sp, $sp, 4
jr $ra
