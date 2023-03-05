# midi-commander-custom
Custom Firmware for the MeloAudio Midi Commander / Harley Benton MP100



# Build status
Fonts build with https://larsee.com/blog/2014/05/converting-fonts-to-c-source-using-bmfont2c/

# Current features list


# Still to come



# Basic instructions for setting up development environment
Other than the simple python scripts, it's all just STM32CubIDE. Install that, import the project into your workspace (it's just shrink wrapped eclipse) and your done.

There's two Build settings, one for the DFU (with offset linker script and vector table) and the other for using with a ST-Link debugger. To build a DFU file for upload you'll need to build the binary in the IDE, then use the DFU packing tool that comes with the DFU uploader (can't remember their exact names off the top of my head.) Using the Intel HEX format file instead of the .bin saves you having to input the flash offset.
