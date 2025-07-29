# MIDI Commander Custom - STM32 CMake/Ninja Setup Guide

This guide explains how to set up, build, and develop the MIDI Commander Custom firmware using CMake, Ninja, and the ARM GNU Toolchain on Windows, without STM32CubeMX.

---

## Prerequisites


1. **ARM GNU Toolchain**
   - Download and install from [Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
   - Example install path: `C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\14.3 rel1\bin`

2. **Ninja Build System**
   - Download from [ninja-build.org](https://ninja-build.org/)
   - Place `ninja.exe` in a folder (e.g., `C:\ninja`) and add this folder to your system `PATH`.

3. **CMake**
   - Download and install from [cmake.org](https://cmake.org/download/)
   - Add CMake to your system `PATH`.

4. **VS Code (optional, recommended)**
   - Install [Visual Studio Code](https://code.visualstudio.com/)
   - Install the "CMake Tools" extension from the VS Code marketplace.

---

## Project Structure

- `MIDI_Commander_Custom/`
  - `CMakeLists.txt` (main build script)
  - `arm-gcc-toolchain.cmake` (toolchain file)
  - `Core/`, `Drivers/`, `Middlewares/`, `USB_DEVICE/` (source and headers)

---

## Setup Steps

### 1. Clone or Download the Repository

```
git clone <your-repo-url>
cd midi-commander-custom/MIDI_Commander_Custom
```

### 2. Add Toolchains to PATH (if not already)
- Add ARM toolchain `bin` directory to your system `PATH`.
- Add Ninja directory (e.g., `C:\ninja`) to your system `PATH`.

### 3. Clean Previous Builds (if any)

```
rd /s /q build
```
Or delete the `build` folder manually.

### 4. Configure the Project with CMake

```
cmake -G "Ninja" -DCMAKE_TOOLCHAIN_FILE=arm-gcc-toolchain.cmake -S . -B build
```

If you did not add Ninja to your `PATH`, specify it directly:
```
cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM=C:/ninja/ninja.exe -DCMAKE_TOOLCHAIN_FILE=arm-gcc-toolchain.cmake -S . -B build


For odd reasons this is needed:
cmake -G "Ninja" -DCMAKE_MAKE_PROGRAM=C:/ninja/ninja.exe -DCMAKE_TOOLCHAIN_FILE=F:/git/stm32/midi-commander-custom/MIDI_Commander_Custom/arm-gcc-toolchain.cmake -S . -B build
```

### 5. Build the Project

```
cmake --build build
```


### 6. Output Files
- The build output (ELF, HEX) will be in the `build` directory.

---

## Cleaning the Build

To remove all build artifacts and ensure a clean build:

**Delete the build directory:**
```
rd /s /q build
```
Or delete the `build` folder manually in File Explorer.

**Or, use CMake's clean target:**
```
cmake --build build --target clean
```
This removes all compiled files but keeps the build directory and CMake cache.

For a completely fresh build, delete the `build` directory and re-run the CMake configuration step.

---


## Programming and Debugging with SEGGER J-Link & Ozone

### Flashing the Firmware

You can use SEGGER J-Link tools to program your STM32 device:

- Use J-Link Commander or J-Flash to flash the `.hex` or `.elf` file in the `build` directory.
- Example command (J-Link Commander):
  ```
  JLinkExe -device STM32F103RE -if SWD -speed 4000 -CommanderScript flash.jlink
  ```
  (Create a `flash.jlink` script with the appropriate `loadfile` and `r`/`g` commands.)

### Debugging with SEGGER Ozone

1. Open Ozone and create a new project.
2. Select your `.elf` file from the `build` directory as the application.
3. Set the device to `STM32F103RE` and interface to SWD.
4. You can use the provided `ozone.jdebug` configuration file for quick setup.
5. Start debugging and set breakpoints as needed.

For more information, see the [SEGGER Ozone documentation](https://www.segger.com/products/debug-probes/j-link/technology/ozone-j-link-debugger/).

---

## Troubleshooting
- If you get errors about missing includes (e.g., `stm32f1xx.h`), ensure you have the STM32F1xx CMSIS device headers in `Drivers/CMSIS/Device/ST/STM32F1xx/Include` and that this path is in your `CMakeLists.txt`.
- If you get generator errors, always delete the `build` directory before switching generators (e.g., from Visual Studio to Ninja).
- If CMake cannot find Ninja, check your `PATH` or use `-DCMAKE_MAKE_PROGRAM`.
- If you get `_exit` or linker errors during CMake configuration, ensure `set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)` is in your toolchain file.

---

## VS Code Integration
- Open the `MIDI_Commander_Custom` folder in VS Code.
- Use the CMake Tools extension to configure and build (select Ninja as the generator and your toolchain file).

---

## License
See LICENSE file for details.

---

For further help, open an issue or contact the maintainer.
