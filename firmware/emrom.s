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
	status = 0x20
	status_write_bit = 0x08 * (status - 0x20) + 0x07
	ack = '!

	.area text (CODE, ABS)

	.org 0x0000

power_on:

	mov 0x8F, #0x01
	mov PCON, #0x80
	mov SCON, #0x90
	setb TI

frame_start:

	mov checksum, #0x00
	mov counter, #0x00

	acall getc
	mov DPH, A
	mov DPL, #0x00
	add A, checksum
	mov checksum, A

	acall getc
	mov status, A
	add A, checksum
	mov checksum, A

	jb status_write_bit, write_memory

read_memory:

	movx A, @DPTR
	acall putc
	add A, checksum
	mov checksum, ACC
	inc DPTR
	djnz counter, read_memory

	ajmp frame_stop

write_memory:

	acall getc
	movx @DPTR, A
	inc DPTR
	add A, checksum
	mov checksum, A
	djnz counter, write_memory

frame_stop:

	mov A, #ack
	acall putc
	add A, checksum
	mov checksum, A

	mov A, checksum
	acall putc
	ajmp frame_start

getc:
	jnb RI, getc
	mov A, SBUF
	clr RI
	ret

putc:
	jnb TI, putc
	clr TI
	mov SBUF, A
	ret
