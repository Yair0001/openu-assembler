OpenU Assembler (University Project, 2023)
=================================================

Overview
--------

This repository contains a simple two-pass assembler implemented as a university project (2023). The assembler processes input assembly files and produces the corresponding machine/code output. The project is provided for educational purposes and demonstrates typical assembler stages (pre-processing, symbol table construction, and final code generation).

Key Features
------------

- Pre-assembler stage to handle macros/cleanup
- First pass to build symbol and instruction tables
- Second pass to resolve symbols and emit final output
- Small, modular C codebase suitable for study and extension

Build
-----

A `Makefile` is included. Build the project by running:

    make

This will compile the source files and produce the assembler executable (see `Makefile` for target name).

Usage
-----

Run the assembled executable with an assembly source file. Example:

    ./assembler test.as

Replace `assembler` with the actual executable name produced by the `Makefile` if different. Several example source files are included: `test.as`, `test2.as`, `test3.as`, and `firstprog.as`.

Repository Structure
--------------------

- `preAssembler.c`, `preAssembler.h` — pre-processing stage (macro expansion/cleanup)
- `firstpass.c`, `firstpass.h` — first assembler pass (symbol table, addresses)
- `secondpass.c`, `secondpass.h` — second assembler pass (resolve symbols, emit output)
- `main.c` — driver that orchestrates the assembly process
- `utils.c`, `utils.h` — helper functions used across modules
- `constants.h` — project-wide constants and definitions
- `Makefile` — build rules
- Example assembly files: `firstprog.as`, `test.as`, `test2.as`, `test3.as`

Notes
-----

- This project was completed as a university assignment in 2023 and is intended for learning and demonstration purposes.
- See inline comments in the source files for implementation details and extension points.

Credits
-------

University assignment (2023). Review and attribution as required by your course guidelines.
