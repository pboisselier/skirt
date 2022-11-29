# SKIRT: Small Kernel in Real Time (or not)

**Work in Progress**

## Features

- [x] Task Management
    - [x] Creation
    - [x] Deletion (and exit)
    - [x] Switching
    - [x] Sleeping (conversion to milliseconds is TODO)
    - [x] Await/Awake
    - [x] Priority-based scheduling (HARD_PRIO)
- [ ] I/Os
    - [x] Basic Serial
    - [ ] Task waiting for I/Os
    - [ ] Hardware Interrupts registration for tasks
- [ ] IPCs
    - [x] Mails & Boxes
    - [x] Semaphores
    - [ ] Signals
- [ ] Support
    - [ ] AVR
        - [x] ATmega328P
    - [ ] ARMv6-M (Cortex-M0+)
        - [ ] RP2040

## Improvement Ideas

- Global
    - [ ] Create a way to make proper "syscall" (`svc/svi` on arm)
- Tasks
    - [ ] Replace sk_task* with a TaskID (tid)?
- IPCs
    - [ ] Add IDs for Semaphores instead of pointer?

# How to use

## Quick-Start

*This quick-start uses an ATmega328P for demonstration.*

### Retrieving the Kernel

The easiest way and preferred way to download sources is to use `git clone` which will create a folder named `skirt`
with everything needed inside.

```shell
git clone https://github.com/pboisselier/skirt.git
```

You can also use the "<> Code" button and select "Download ZIP" if you prefer.

### Building Examples

This project uses [CMake](https://cmake.org/) to provide ease-of-use across platforms.
First thing is to make sure you have `cmake` installed, if not you can download it
from [here](https://cmake.org/download/).

Next, you need to have the GCC AVR Toolchain installed and in your `$PATH`, this can be checked easily:

```shell
avr-gcc --version

# Possible Output
#avr-gcc (GCC) 7.3.0
#Copyright (C) 2017 Free Software Foundation, Inc.
#This is free software; see the source for copying conditions.  There is NO
#warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

If you need to install `avr-gcc`, use your favourite package manager or WSL on Windows (or use the Microchip provided
one but this can require changing the CMakeLists.txt to use `xc8`).
For *Apple M1*, homebrew does not have the AVR GCC Toolchain available, however you can find it on the Microchip
website,
or use PlatformIO which will retrieve it for you.

Now you can build using CMake, you need to create an empty folder named "build" which will be used to hold temporary
files.

```shell
# Make sure you are at the root of the skirt folder.
mkdir build
cd build
```

This project requires some options to be passed on to CMake, however it will not work.

```shell
# This will build all examples for the ATmega328P in Debug mode
cmake .. -DSKIRT_EXAMPLES=1 -DSKIRT_ARCH=avr -DSKIRT_AVR_MCU=atmega328p -DCMAKE_BUILD_TYPE=Debug

# Possible Output:
#-- The C compiler identification is [...]
#-- The CXX compiler identification is [...]
#-- Detecting C compiler ABI info
#-- Detecting C compiler ABI info - done
#-- Check for working C compiler: [...] - skipped
#-- Detecting C compile features
#-- Detecting C compile features - done
#-- Configuring done
#-- Generating done
#-- Build files have been written to: ./build
```

Finally, we can use `make` (or `ninja` if CMake preferred it) to build all targets.

```shell
make

# Possible Output
# [ 11%] Building C object CMakeFiles/skirt.dir/src/sk/skirt.c.obj
# [ 22%] Building C object CMakeFiles/skirt.dir/src/sk/task.c.obj
# [ 33%] Building C object CMakeFiles/skirt.dir/src/sk/ipc.c.obj
# [ 44%] Building C object CMakeFiles/skirt.dir/src/sk/arch/avr/avr.c.obj
# [ 55%] Linking C static library libskirt.a
# [ 55%] Built target skirt
# [ 66%] Building C object CMakeFiles/semaphores.elf.dir/src/examples/semaphores.c.obj
# [ 77%] Linking C executable semaphores.elf
# [ 77%] Built target semaphores.elf
# [ 88%] Building C object CMakeFiles/mails.elf.dir/src/examples/mails.c.obj
# [100%] Linking C executable mails.elf
# [100%] Built target mails.elf
```

There should be multiple `.elf` files in the `build` folder.

### Uploading an example

You need to have `avrdude` installed first (it comes with Arduino, so you can find it in Arduino's install folder if
needed).

*You need to replace `example.elf` in the commands below with the `.elf` file you want to upload.*

#### For Arduino

```shell
avrdude -p m328p -c arduino -P /arduino/serial/port -b115200 -U flash:w:example.elf
```

You need to replace `/arduino/serial/port` with the actual path to the serial port (`/dev/ttyACM0` on linux for
instance).

#### For Microchip's xPlainedMini

No need to specify a serial port as it's using USB.

```shell
avrdude -p m328p -c xplainedmini -U flash:w:example.elf
```

## Options

### Kernel Memory Allocation

By default, this kernel uses a static allocation scheme for tasks and IPCs.

When using `SKIRT_ALLOC_STATIC`:

- `SKIRT_TASK_MAX`, defines how many tasks can exist at the same time.
- `SKIRT_SEM_MAX`, defines how many semaphores can exist at the same time.
- `SKIRT_MAIL_MAX`, defines how many mails can exist at the same time.

*Note: if `SKIRT_SEM_MAX` and/or `SKIRT_MAIL_MAX` are not specified, `SKIRT_TASK_MAX` is used (5 by default)!*

### Others

- `SKIRT_SERIAL_BAUD`, by default the baud rate is set to 115200, you can change it by setting this macro.
- `SKIRT_HARD_PRIO`, this enables struct priority-based scheduling, all tasks yield to the highest priority one until it
  needs to sleep or wait. In the case of multiple tasks having the same priority, the last one created is preferred.
- `SKIRT_DEBUG`, will add information and more assertions (increase memory by a lot!).
- `SKIRT_VANITY`, prettier debug messages (increases memory use on certain MCUs).
- `SKIRT_PREEMPT_TIME`, arbitrary value for preemption timer.
- `SKIRT_TASK_STACK_SZ`, provides a default stack size for a specific architecture, this can be changed if **needed**.
- `SKIRT_KERNEL`, enables kernel symbol export (used for building `libskirt`).

## Compiling

When compiling, CMake expects some options:

- `SKIRT_ARCH` , which is the targeted architecture
- `SKIRT_CC_PREFIX`, which provides a path for a crosscompiler, `path/prefix-` (the `-` at the
  end is important).
- `SKIRT_EXAMPLES`, will build some examples ELF files ready to be uploaded and tested.

For debugging, use `CMAKE_BUILD_TYPE=Debug` to include debugging symbols and other options.

### AVR

For instance using a ATmega328P (which is the only AVR supported for the moment):

```shell
mkdir build
cd build
cmake .. -DSKIRT_ARCH=avr -DSKIRT_AVR_MCU=atmega328p -DCMAKE_BUILD_TYPE=Debug
make
```

## Troubleshooting

### Checking object file

For some MCU like the ATmega328P where there is only 2KiB of SRAM, the generated ELF file may be too big to fit in.
This can lead to lots of undefined and weird behaviours.
You can use the `avr-size` to check:

```shell
avr-size -C --mcu=atmega328p -x test.elf

AVR Memory Usage
----------------
Device: atmega328p

Program:    4276 bytes (13.0% Full)
(.text + .data + .bootloader)

Data:       1923 bytes (93.9% Full)
(.data + .bss + .noinit)
```

To reduce size, remove unnecessary strings as they tend to be stored in SRAM on this platform.
You can also build without Debug, which will greatly reduce string consumption by removing unneeded information in
assertions. 
