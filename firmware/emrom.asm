	.EQU size, 0x20
	.EQU buffer, 0x3E

	.FLAG LE0, P3.2
	.FLAG LE1, P3.3
	.FLAG AEN, P3.5
	.FLAG MRD, P3.6
	.FLAG MWR, P3.7

	.ORG 0x0000

entry:
	mov TL1, #0xFF
	mov TH1, #0xFF
	mov TMOD, #0x20
	mov TCON, #0x40
	mov PCON, #0x80
	mov SCON, #0x52

loop:
	clr LE0
	setb LE1
	clr AEN
	clr MRD
	setb MWR
	mov P0, #0xFF
	mov P1, #0xFF
	mov P2, #0xFF
	acall recv
	mov A, size
	
read:
	cjne A, #0x02, write
	setb LE0
	clr LE1
	setb AEN
	setb MRD
	setb MWR
	mov DPL, buffer
	mov DPH, buffer + 1
	mov R0, #(buffer + 2)
	mov R1, #0x40

read_data:	
	mov P1, DPL
	mov P2, DPH
	clr MRD
	nop
	mov @R0, P0
	setb MRD
	inc R0
	inc DPTR
	djnz R1, read_data
	mov size, #0x42
	acall send
	ajmp loop
	
write:
	cjne A, #0x42, loop
	setb LE0
	clr LE1
	setb AEN
	setb MRD
	setb MWR
	mov DPL, buffer
	mov DPH, buffer + 1
	mov R0, #(buffer + 2)
	mov R1, #0x40

write_data:
	mov P1, DPL
	mov P2, DPH
	mov P0, @R0
	nop
	clr MWR
	nop
	setb MWR
	inc R0
	inc DPTR
	djnz R1, write_data
	mov size, #0x02
	acall send
	ajmp loop

;-------------------------------

recv:
	mov R0, #buffer
	mov R1, #0

recv_head:
	acall get
	cjne A, #':', recv_head

recv_data:
	acall get
	cjne A, #'\n', recv_data_high

recv_tail:
	mov size, R1
	ret

recv_data_high:
	acall decode
	jc recv
	swap A
	mov @R0, A
	nop

recv_data_low:
	acall get
	acall decode
	jc recv
	orl A, @R0
	mov @R0, A
	inc R0
	inc R1
	cjne R1, #0x42, recv_data
	ajmp recv_tail

;-------------------------------

decode:
	cjne A, #'0', decode_pass_0
	
decode_pass_0:	
	jc decode_bad
	cjne A, #('9' + 1), decode_pass_9
	
decode_pass_9:
	jc decode_0_to_9
	cjne A, #'A', decode_pass_A
	
decode_pass_A:	
	jc decode_bad
	cjne A, #('F' + 1), decode_pass_F
	
decode_pass_F:
	jc decode_A_to_F

decode_bad:
	setb C
	ret	

decode_0_to_9:
	add A, #(-'0')
	clr C
	ret	

decode_A_to_F:
	add A, #(0x0A - 'A')
	clr C
	ret	

;-------------------------------

send:
	mov R0, #buffer
	mov R1, size

send_head:
	mov A, #':'
	acall put

send_data:
	cjne R1, #0x00, send_data_high

send_tail:
	mov A, #'\n'
	acall put
	ret

send_data_high:
	mov A, @R0
	swap A
	acall encode
	acall put

send_data_low:
	mov A, @R0
	acall encode
	acall put

	inc R0
	dec R1

	ajmp send_data

;-------------------------------

encode:
	anl A, #0x0F
	cjne A, #0x0A, encode_pass 

encode_pass:
	jc encode_0_to_9

encode_A_to_F:
	add A, #('A' - 0x0A)
	ret

encode_0_to_9:
	add A, #'0'
	ret	

;-------------------------------

get:
	jnb RI, get
	mov A, SBUF
	clr RI
	ret

;-------------------------------

put:
	jnb TI, put
	clr TI
	mov SBUF, A
	ret

	.END

