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

## How to use

Tasks must first be created using `sk_task_create_static`.
Then a call to `sk_kernel_start` starts the kernel and tasks will be executed from there.

### Options

#### Kernel Memory Allocation

By default, this kernel uses a static allocation scheme for tasks and IPCs.

When using `SKIRT_ALLOC_STATIC`:

- `SKIRT_TASK_MAX`, defines how many tasks can exist at the same time.
- `SKIRT_SEM_MAX`, defines how many semaphores can exist at the same time.
- `SKIRT_MAIL_MAX`, defines how many mails can exist at the same time.

*Note: if `SKIRT_SEM_MAX` and/or `SKIRT_MAIL_MAX` are not specified, `SKIRT_TASK_MAX` is used (5 by default)!*

#### Others

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
