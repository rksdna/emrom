#
# Emrom - ROM emulator firmware
# Copyright (c) 2016 rksdna
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Project files

TARGET = emrom
SRC = $(TARGET).asm
LST = $(TARGET).lst
HEX = $(TARGET).hex

PORT = /dev/parport0

# Targets list

.PHONY: all clean install

# Default target

all: $(HEX)

$(HEX): $(SRC)
	@echo "Linkning $@..."
	as31 -l -O$@ $^

install: $(HEX)
	@echo "Installing $@..."	
	at89-isp -a direct -t at89s51 -c $(PORT) -e -w $@ -d

clean:
	@echo "Clean..."
	rm -f $(HEX) $(LST)
	
