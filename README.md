# Oxyb

Oxyb is a minimal and hobby kernel written in ANSI C. Its goal is to be the most minimal yet functional microkernel possible for the AMD64 architecture.

## Why?

- Why ANSI? I want the code to be as portable as possible, without using GNU extensions like inline assembly.
- Why microkernel? There is a false belief that microkernels are slow. While this could be true in the 1990s, with the power of newest PCs, microkernels are as fast as any other kernel model. Microkernels are also able to control which resources are used at every moment because there are less built-in features in the kernel.
- Why OSDev? Because it's fun and you can learn a lot of how a computer works.

## Current progress

- [X] Basic setup, GDT, IDT and logging
- [X] Memory management, mapping and heap
- [X] ACPI and APIC
- [ ] PCI devices
- [ ] AHCI and NVMe drivers
- [ ] FAT32 driver

## License

This project is licensed under MIT License.
 
