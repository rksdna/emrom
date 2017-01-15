#Emrom

Simple 8 bit 64 Kbyte ROM emulator for controllers with external memory.

![Image](/hardware/board.png)

Building loader:
```
cd emrom/software
make all
sudo make install
make clean
```

Load file to emulator:
```
emrom -c /dev/ttyS0  -w file.hex -d
```


