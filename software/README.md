# Software Files #

- firmware
- etc.

# Development Environment 

- IDE: Eclipse Helios
- Eclipse plugins for ChibiOS: (copy to `/plugins` directory)
 - org.chibios.tools.eclipse.config_1.2.1.jar
 - org.chibios.tools.eclipse.debug_1.0.8.jar
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
