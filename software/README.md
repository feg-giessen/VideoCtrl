# Software Files #

- firmware
- etc.

# Development Environment 

- IDE: Eclipse Helios
- Compiler: arm-none-eabi (v4.7 - https://launchpad.net/gcc-arm-embedded/+download) 
- binutils (make, etc.): msys 1.0 (MinGW)

## Debug Configuration

### Init

    target remote localhost:3333
    monitor reset halt
    monitor wait_halt
    monitor sleep 100
    monitor poll
    monitor flash write_image erase build/videoctrl.bin 0x08000000
    file build/videoctrl.elf
    monitor sleep 200

### Run

    monitor soft_reset_halt
    monitor wait_halt
    monitor poll
    thbreak main
    continue

# Setup

- unpack `.\ChibiOs\ext\lwip-1.4.1_patched.zip`
