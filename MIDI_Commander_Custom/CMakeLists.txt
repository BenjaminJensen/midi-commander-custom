# Top-level CMake file for MIDI Commander Custom (STM32F103RETx)
cmake_minimum_required(VERSION 3.13)
project(MIDI_Commander_Custom C ASM)

# Set the target MCU and toolchain
set(MCU cortex-m3)
set(MCU_DEVICE STM32F103xE)
set(MCU_FAMILY STM32F1xx)
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/STM32F103RETX_FLASH.ld)

# Set the cross-compiler

# Project source files (add all .c files)
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/Core/Src/*.c
    ${CMAKE_SOURCE_DIR}/Drivers/**/*.c
    ${CMAKE_SOURCE_DIR}/Middlewares/**/*.c
    ${CMAKE_SOURCE_DIR}/USB_DEVICE/**/*.c
)
file(GLOB_RECURSE ASM_SOURCES
    ${CMAKE_SOURCE_DIR}/Core/Startup/*.s
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Drivers
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F1xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/Config
    ${CMAKE_SOURCE_DIR}/Drivers/RTT
    ${CMAKE_SOURCE_DIR}/Drivers/file_system
    ${CMAKE_SOURCE_DIR}/Middlewares
    ${CMAKE_SOURCE_DIR}/Middlewares/draw
    ${CMAKE_SOURCE_DIR}/Middlewares/ssd1306
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc
    ${CMAKE_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/MIDI/Inc
    ${CMAKE_SOURCE_DIR}/USB_DEVICE/App
    ${CMAKE_SOURCE_DIR}/USB_DEVICE/Target
)

# Defines
add_definitions(-D${MCU_DEVICE} -DUSE_HAL_DRIVER)

# Compiler flags
set(CMAKE_C_FLAGS "${CPU_FLAGS} -Wall -O2 -ffunction-sections -fdata-sections -g")
set(CMAKE_ASM_FLAGS "${CPU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${CPU_FLAGS} -T${LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map=${PROJECT_NAME}.map -g")

# Output ELF and HEX
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${ASM_SOURCES})
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
)

# Print message
message(STATUS "CMake STM32 project configured for ${MCU_DEVICE}")
