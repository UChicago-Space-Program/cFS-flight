#BeagleBone Black (AM335x, ARMv7-A hard-float) cross toolchain
#Cross system
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_SYSTEM_VERSION 1)
set(CSP_ENABLE_SOCKETCAN ON CACHE BOOL "" FORCE)

#Compilers (Debian/Ubuntu cross packages: gcc-arm-linux-gnueabihf, g++)
set(CMAKE_C_COMPILER   "/usr/bin/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/arm-linux-gnueabihf-g++")

#Use a real sysroot (pick one):
#1) Host cross libs:
#set(CMAKE_SYSROOT "/usr/arm-linux-gnueabihf")
#2) Preferred: copy of BBB rootfs (recommended for consistent headers/libs):
#set(CMAKE_SYSROOT "/opt/sysroots/bbb-rootfs")*/
#Make find_* search inside the sysroot for headers/libs
set(CMAKE_FIND_ROOT_PATH         "${CMAKE_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

#Don’t try to run test binaries during checks
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#CPU/ABI tuning for BBB
set(CMAKE_C_FLAGS_INIT   "-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=hard")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_C_FLAGS_INIT}")

#Ensure the linker also uses the sysroot
set(CMAKE_EXE_LINKER_FLAGS_INIT    "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "--sysroot=${CMAKE_SYSROOT}")

#If you use pkg-config from the host, point it at the sysroot
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")
set(ENV{PKG_CONFIG_LIBDIR}      "${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf:${CMAKE_SYSROOT}/usr/lib:${CMAKE_SYSROOT}/lib")

#cFS/OSAL selections
set(CFE_SYSTEM_PSPNAME "pc-linux")
set(OSAL_SYSTEM_OSTYPE "posix")


# where is the target environment
#SET(CMAKE_FIND_ROOT_PATH		"/bin/arm-linux-gnueabi-")
SET(CMAKE_FIND_ROOT_PATH "/usr/arm-linux-gnueabihf")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM	NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY	ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE	ONLY)

# these settings are specific to cFE/OSAL and determines which 
# abstraction layers are built when using this toolchain
# Note that "pc-linux" works fine even though this is not technically a "pc"
SET(CFE_SYSTEM_PSPNAME      "pc-linux")
SET(OSAL_SYSTEM_OSTYPE      "posix")

# SET(CMAKE_C_FLAGS_INIT "-march=armv7" CACHE STRING "C Flags required by platform")
