/*
 * Emrom Loader
 * Copyright (c) 2016 Andrey Skrypka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "serial.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

HANDLE port = INVALID_HANDLE_VALUE;

int serial_open(const char* name)
{
    DCB dcb =
    {
        sizeof(DCB), CBR_115200, TRUE, FALSE, FALSE, FALSE,
        DTR_CONTROL_DISABLE, FALSE,  FALSE, FALSE, FALSE,
        FALSE, FALSE, RTS_CONTROL_DISABLE, FALSE, 0, 0,
        2048, 512, 8, EVENPARITY, ONESTOPBIT, 0x00, 0x00, 0x00,
        0x00, 0x00, 0
    };
    COMMTIMEOUTS timeouts =
    {
        0, 0, 250, 0, 250
    };
    serial_close();
    port = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (port == INVALID_HANDLE_VALUE)
        return SERIAL_FILE_ERROR;
    SetCommState(port, &dcb);
    SetCommTimeouts(port, &timeouts);
    return 0;
}

void serial_close()
{
    if (port != INVALID_HANDLE_VALUE)
    {
        CloseHandle(port);
        port = INVALID_HANDLE_VALUE;
    }
}

int serial_control(int rts, int dtr)
{
    if (!EscapeCommFunction(port, rts ? SETRTS : CLRRTS))
        return SERIAL_IO_ERROR;
    if (!EscapeCommFunction(port, dtr ? SETDTR : CLRDTR))
        return SERIAL_IO_ERROR;
    return 0;
}

int serial_clear()
{
    if (!PurgeComm(port, PURGE_TXCLEAR | PURGE_RXCLEAR))
        return SERIAL_IO_ERROR;
    return 0;
}

int serial_write(const void* buffer, int size)
{
    DWORD count;
    if (!WriteFile(port, buffer, size, &count, NULL))
        return SERIAL_IO_ERROR;
    if (count != size)
        return SERIAL_IO_ERROR;
    return 0;
}

int serial_read(void* buffer, int size)
{
    DWORD count;
    if (!ReadFile(port, buffer, size, &count, NULL))
        return SERIAL_IO_ERROR;
    if (count != size)
        return SERIAL_IO_ERROR;
    return 0;
}

void serial_wait(int ms)
{
    DWORD begin = GetTickCount();
    while (GetTickCount() - begin < ms)
        continue;
}

#elif defined(__linux__)

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int fd = -1;

int serial_open(const char* name)
{
    struct termios options;
    serial_close();
    fd = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
        return SERIAL_FILE_ERROR;
    if (fcntl(fd, F_SETFL, 0) == -1)
        return SERIAL_IO_ERROR;
    if (tcgetattr(fd, &options) == -1)
        return SERIAL_IO_ERROR;
    if ((cfsetispeed(&options, B115200) == -1) || (cfsetospeed(&options, B115200) == -1))
        return SERIAL_IO_ERROR;
    cfmakeraw(&options);
    options.c_cflag = B115200 | PARENB | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNBRK | IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 5;
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &options) == -1)
        return SERIAL_IO_ERROR;
    return 0;
}

void serial_close()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int serial_control(int rts, int dtr)
{
    int status;
    if (ioctl(fd, TIOCMGET, &status) == -1)
        return SERIAL_IO_ERROR;
    status &= ~(TIOCM_RTS | TIOCM_DTR);
    if (rts)
        status |= TIOCM_RTS;
    if (dtr)
        status |= TIOCM_DTR;
    if (ioctl(fd, TIOCMSET, &status) == -1)
        return SERIAL_IO_ERROR;
    return 0;
}

int serial_clear()
{
    if (tcflush(fd, TCIOFLUSH))
        return SERIAL_IO_ERROR;
    return 0;
}

int serial_write(const void* buffer, int size)
{
    int count = write(fd, buffer, size);
    if (count != size)
        return SERIAL_IO_ERROR;
    return 0;
}

int serial_read(void* buffer, int size)
{
    int n = 0, count;
    while (n < size)
    {
        count = read(fd, buffer, size - n);
        if (count <= 0)
            return SERIAL_IO_ERROR;
        n += count;
        buffer += count;
    }
    return 0;
}

void serial_wait(int ms)
{
    usleep(1000 * ms);
}

#else
#error "Unsupported architecture"
#endif

