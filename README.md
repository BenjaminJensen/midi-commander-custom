# midi-commander-custom
Custom Firmware for the MeloAudio Midi Commander / Harley Benton MP100
Forked from the harvie256/midi-commander-custom, where a log of good work has been done on the hardware side.

However as I am a guitarist I needed a more old-school solotion alla Bradshaw / Rocktron controllers like the CAE RS10 / Rocktron Midi-Mate, where you have bankc of Program Change and Controller Change, with easy acces to Instant Access Switches for a pedalboard expierence.


# Build status
Fonts build with https://larsee.com/blog/2014/05/converting-fonts-to-c-source-using-bmfont2c/

# Current features list
- Display working
- Buttons working
- LEDs working
- Event system working
- MIDI out working
- Preset Structure
- IA structure with 3 pages of 8 switches
- Settings system inplace

# Still to come
- Actual saving and loading of presets
- Mode switching [Preset, IA0, IA1, IA2, Edit]
- Edit mode missing
- USB: no focus on USB as it is not needed. It could however be used for host interfacing MIDI pedals

# Basic instructions for setting up development environment
Other than the simple python scripts, it's all just STM32CubIDE. Install that, import the project into your workspace (it's just shrink wrapped eclipse) and your done.

Segger JLink and RTT have been added to the project for easy debugging.

There's two Build settings, one for the DFU (with offset linker script and vector table) and the other for using with a ST-Link debugger. To build a DFU file for upload you'll need to build the binary in the IDE, then use the DFU packing tool that comes with the DFU uploader (can't remember their exact names off the top of my head.) Using the Intel HEX format file instead of the .bin saves you having to input the flash offset.
