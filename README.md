# SKIRT: Small Kernel in Real Time (or not)

**Work in Progress**

## Features

- [ ] Task Management
    - [x] Creation
    - [x] Deletion (and exit)
    - [x] Switching
    - [x] Sleeping (conversion to milliseconds is TODO)
    - [ ] Priority-based scheduling
- [ ] I/Os
    - [x] Basic Serial
    - [ ] Task waiting for I/Os
    - [ ] Hardware Interrupts registration for tasks
- [ ] IPCs
    - [ ] Message queues
    - [ ] Semaphores
- [ ] Support
    - [ ] AVR
        - [x] ATmega328P
    - [ ] ARMv6-M (Cortex-M0+)
        - [ ] RP2040

## How to use

Tasks must first be created using `sk_task_create_static`.
Then a call to `sk_kernel_start` starts the kernel and tasks will be executed from there.

## Compiling

For the moment, only a debug testing build is available.
You can specify `-DSKIRT_CC_PREFIX=/path/to/cc/prefix-` (the `-` at the end is important).

```shell
mkdir build
cd build
cmake .. -DSKIRT_ARCH=avr -DSKIRT_AVR_MCU=atmega328p -DCMAKE_BUILD_TYPE=Debug
make
```