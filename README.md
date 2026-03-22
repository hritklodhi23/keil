# Keil MDK – Modern Embedded Systems Programming

Learning embedded systems with Keil MDK uVision, following
[Miro Samek's "Modern Embedded Systems Programming" course](https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM).

---

## Hardware

- **Board:** [TivaC LaunchPad (EK-TM4C123GXL)](https://www.ti.com/tool/EK-TM4C123GXL)
- **MCU:** TM4C123GH6PM (ARM Cortex-M4F, 80 MHz)

---

## Software Requirements

- [Keil MDK uVision 5 (MDK v6 Community Edition)](https://www.keil.arm.com) – free
- Keil Device Family Pack: **Keil.TM4C_DFP** v1.1.0 (install via Pack Installer in uVision)
- [USB drivers for TivaC LaunchPad (Stellaris ICDI)](https://www.state-machine.com/course/MDK_Stellaris_ICDI_AddOn.exe)

---

## Setting Up Keil uVision (matching Miro Samek's lecture)

### 1. Install Keil MDK
Download and install [Keil MDK uVision](https://www.keil.arm.com).

### 2. Install the TM4C Device Pack
1. Open uVision → **Pack Installer** (toolbar icon or `Project › Manage › Pack Installer`).
2. In the **Devices** pane, search for `TM4C123GH6PM` (Texas Instruments).
3. Install **Keil::TM4C_DFP** (version 1.1.0 or later).

### 3. Open the Lesson Project
1. Open uVision.
2. `File › Open Project…` → navigate to `lesson-01/simulator-keil/lesson.uvprojx`.
3. The project opens with:
   - **Target:** `Debug` (ARM Cortex-M4, TM4C123GH6PM)
   - **Source group:** `main.c`
   - **Device group:** startup (`startup_TM4C123.s`) and system (`system_TM4C123.c`) files managed via RTE

### 4. Build the Project
Press **F7** (or `Project › Build Target`). The build should complete with 0 errors and 0 warnings.

### 5. Debug in the Simulator
1. Go to `Project › Options for Target… › Debug` tab.
2. Select **Use Simulator** (left radio button) — matches the Miro Samek lecture setup.
3. Press **Ctrl+F5** (or `Debug › Start/Stop Debug Session`) to start the simulator.
4. Use **Step Into** (F11) to step through `main()` and watch the `counter` variable increment in the Watch window.

---

## Project Structure

```
lesson-01/
└── simulator-keil/
    ├── main.c                          # Lesson source file
    ├── lesson.uvprojx                  # Keil uVision project file
    ├── lesson.uvoptx                   # Keil uVision options (simulator, debug windows)
    └── RTE/
        └── Device/
            └── TM4C123GH6PM/
                ├── startup_TM4C123.s  # ARM Cortex-M4 startup/vector table
                └── system_TM4C123.c   # System clock initialization
```

---

## References

- [Modern Embedded Systems Programming – YouTube playlist](https://www.youtube.com/playlist?list=PLPW8O6W-1chwyTzI3BHwBLbGQoPFxPAPM)
- [QuantumLeaps course repository](https://github.com/QuantumLeaps/modern-embedded-programming-course)
- [Tiva TM4C123GH6PM Data Sheet](https://www.state-machine.com/course/TM4C123GH6PM_Datasheet.pdf)
