# Toolchain file for STM32F103RETx using arm-none-eabi-gcc
set(TOOLCHAIN_BIN "C:/Program Files (x86)/Arm GNU Toolchain arm-none-eabi/14.3 rel1/bin")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Specify the cross compiler
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# Compiler and linker flags
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS_INIT "${CPU_FLAGS} -Wall -O2 -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -Wall -O2 -ffunction-sections -fdata-sections")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -Wl,--gc-sections")

# Prevent CMake from linking with standard system libraries
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
