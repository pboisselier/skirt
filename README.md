# SKIRT: Small Kernel in Real Time (or not)

**Work in Progress**

## Features

- [ ] Task Management
    - [x] Creation
    - [x] Deletion (and exit)
    - [x] Switching
    - [x] Sleeping (conversion to milliseconds is TODO)
    - [x] Await/Awake
    - [ ] Priority-based scheduling
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

## Troubleshooting

### Checking object file

For some MCU like the ATmega328P where there is only 2KiB of SRAM, the generated ELF file may be too big to  fit in.
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
