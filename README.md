# Keil µVision — Miro Samek Configuration

Learning embedded systems following Miro Samek's
[Modern Embedded Systems Programming](https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM)
YouTube course (also covered in his book *Practical UML Statecharts in C/C++, 2nd ed.*).

---

## 1. Target Hardware

| Setting | Value |
|---------|-------|
| Board | Texas Instruments EK-TM4C123GXL (Tiva C LaunchPad) |
| Device | TM4C123GH6PM |
| Core | ARM Cortex-M4F (with FPU) |
| System clock (XTAL) | 16 MHz (on-board crystal) |

---

## 2. Device / Target Settings

**Project → Options for Target → Device tab**
- Select **Texas Instruments → TM4C123GH6PM**

**Project → Options for Target → Target tab**

| Field | Value |
|-------|-------|
| Xtal (MHz) | 16.0 |
| Operating system | None |
| Use MicroLIB | *(unchecked)* |
| Use Cross-Module Optimization | *(unchecked)* |
| IROM1 Start | 0x00000000 |
| IROM1 Size | 0x00040000 (256 KB flash) |
| IRAM1 Start | 0x20000000 |
| IRAM1 Size | 0x00008000 (32 KB SRAM) |

---

## 3. Output Settings

**Project → Options for Target → Output tab**

| Setting | Value |
|---------|-------|
| Create Executable | ✔ checked |
| Debug Information | ✔ checked |
| Create HEX File | ✔ checked |
| Browse Information | ✔ checked |

---

## 4. C/C++ Compiler Settings

**Project → Options for Target → C/C++ tab**

| Setting | Value |
|---------|-------|
| Optimization | Level 0 (`-O0`) — keeps all variables visible in debugger |
| One ELF Section per Function | ✔ checked (`--split_sections`) |
| Strict ANSI C | ✔ checked |
| Enum Container always int | ✔ checked |
| Plain Char is Signed | ✔ checked |
| Warnings | **All Warnings** |
| C99 Mode | ✔ checked |

**Define** (in the *Define:* field):
```
TM4C123GH6PM
```

**Include Paths:** add any project-local header directories, e.g.:
```
.\inc
```

---

## 5. Assembler Settings

**Project → Options for Target → Asm tab**

| Setting | Value |
|---------|-------|
| Define | *(same as C/C++ defines above)* |
| Warnings | All Warnings |

---

## 6. Linker Settings

**Project → Options for Target → Linker tab**

| Setting | Value |
|---------|-------|
| Use Memory Layout from Target Dialog | ✔ checked |
| R/O Base | 0x00000000 |
| R/W Base | 0x20000000 |
| Don't Search Standard Libraries | *(unchecked)* |

Leave the scatter file field **empty** (let Keil auto-generate from the Target tab values).

---

## 7. Debug / Programmer Settings

**Project → Options for Target → Debug tab**

| Setting | Value |
|---------|-------|
| Use | **Stellaris ICDI** (built-in USB debugger on the LaunchPad) |
| Load Application at Startup | ✔ checked |
| Run to main() | ✔ checked |

**Project → Options for Target → Utilities tab**

| Setting | Value |
|---------|-------|
| Use Target Driver for Flash Programming | ✔ checked |
| Driver | **Stellaris ICDI** |

---

## 8. Editor / Environment Settings

**Edit → Configuration** (global IDE preferences Samek uses):

| Setting | Value |
|---------|-------|
| Tab size | 4 spaces |
| Insert spaces for tabs | ✔ checked |
| Auto Indent | ✔ checked |
| View White Space | *(optional, helpful)* |
| Encoding | UTF-8 |

---

## 9. Coding Conventions (Samek Style)

Miro Samek follows these practices in his course code:

- **Naming**: `SCREAMING_SNAKE_CASE` for macros/constants; `lowerCamelCase` for local variables; `UpperCamelCase` for types/classes.
- **Header guards**: traditional `#ifndef MY_FILE_H / #define MY_FILE_H` guards.
- **No dynamic memory**: `malloc`/`free` are never used; all objects are statically allocated.
- **Minimal use of the C standard library** to stay close to bare-metal.
- **`volatile`** keyword used explicitly on all hardware-register variables.
- Source files: `*.c` / `*.h` pairs; one module per file.

---

## 10. Reference Links

| Resource | URL |
|----------|-----|
| Modern Embedded Systems Programming (YouTube) | https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM |
| Source code for the course | https://github.com/QuantumLeaps/modern-embedded-programming-course |
| QP/C framework | https://www.state-machine.com/qpc |
| Keil MDK download | https://www.keil.com/demo/eval/arm.htm |
| TM4C123GH6PM datasheet | https://www.ti.com/product/TM4C123GH6PM |
