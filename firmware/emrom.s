;
; Emrom firmware
; Copyright (c) 2016 Andrey Skrypka
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.
;

	checksum = 0x00
	counter = 0x01

	.area text (CODE, ABS)

	.org 0x0000

power_on:
	mov PCON, #0x80
	mov SCON, #0x52
	mov TL1, #0xFF
	mov TH1, #0xFF
	mov TMOD, #0x20
	mov TCON, #0x40

user_mode:

	mov checksum, #0x00
	mov counter, #0x00

	mov P0, #0xFF
	mov P1, #0xFF
	mov P2, #0xFF
	mov P3, #0x9B

	acall get_byte
	mov P0, #0x00
	mov P1, #0x00
	mov P2, A
	mov P3, #0xF7

boot_mode:

	acall get_byte
	mov P0, A
	clr P3.7
	setb P3.7
	inc P1
	djnz counter, boot_mode

	mov A, checksum
	acall put_byte
	ajmp user_mode

get_byte:
	jnb RI, get_byte
	mov A, SBUF
	xch A, checksum
	add A, checksum
	xch A, checksum
	clr RI
	ret

put_byte:
	jnb TI, put_byte
	clr TI
	mov SBUF, A
	ret

