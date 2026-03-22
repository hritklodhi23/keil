# Keil µVision setup (match the lecture)

This guide helps you configure **Keil µVision** to match the layout and target options shown in the lecture.
If you see a setting in the video that is different in your IDE, update it to match using the steps below.

## 1) Install required device packs
1. Open **Keil uVision**.
2. Go to **Pack Installer**.
3. Search for the **exact device family** used in the lecture.
4. Install the device pack(s) shown in the lecture.

## 2) Create or open the project
1. **Project → New uVision Project...**
2. Select the **same device** as in the lecture (exact part number).
3. In **Manage Run‑Time Environment**, enable the same components shown in the lecture.
   - Typical minimum: **CMSIS → Core** and **Device → Startup**.

> If you already have a project, open it and continue to the next section.

## 3) Match Target Options (Project → Options for Target)
Open **Project → Options for Target** and adjust these tabs to match the lecture.

### Target tab
- **Device**: Same as the lecture
- **Xtal (MHz)**: Match the lecture’s clock frequency
- **Use MicroLIB**: Enable only if the lecture shows it

### Output tab
- **Create HEX File**: Enable if the lecture enables it
- **Browse Information**: Enable if the lecture enables it

### C/C++ tab
- **Language / C Standard**: Match the lecture (e.g., C99/C11)
- **Optimization**: Match the lecture (often **Level 0** for debugging)
- **Define**: Add any preprocessor defines shown in the lecture

### Linker tab
- **Use Memory Layout from Target Dialog**: Match the lecture
- **Scatter File**: Only if the lecture uses a custom scatter file

### Debug tab
- **Use**: Select the same debug driver (e.g., **Simulator**, **ST‑Link**, **ULINK**)
- **Settings**: Match the lecture’s debug settings

## 4) Match the µVision layout (windows/toolbars)
The lecture typically shows these panels open. Enable them if missing:

- **View → Project Window**
- **View → Output Window**
- **View → Disassembly Window** (if used)
- **View → Watch Windows**
- **View → Registers** / **Peripherals** (in debug mode)

If your layout is messy, use **Window → Reset Window Layout** and re‑open the windows listed above.

## 5) Quick lecture‑match checklist
Use this list while watching the lecture and check items off as you match them:

- [ ] Device part number matches
- [ ] Xtal frequency matches
- [ ] Runtime Environment components match
- [ ] HEX file option matches
- [ ] Optimization level matches
- [ ] Debug driver matches
- [ ] Layout/windows match

## Need a specific setting?
If a setting in the lecture is still unclear, open **Options for Target** and compare each tab with the lecture video.
The names in the lecture should match the names in the dialogs.
