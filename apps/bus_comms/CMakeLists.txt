cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0079 NEW)
project(CFE_BUS_COMMS_APP C)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ENABLE_CAN ON CACHE BOOL "" FORCE)
set(ENABLE_CAN_SOCKETCAN ON CACHE BOOL "" FORCE)
set(CSP_ENABLE_SOCKETCAN ON CACHE BOOL "" FORCE)

# Find libsocketcan before adding subdirectory so autoconfig.h is generated correctly
find_package(PkgConfig)

if(PKG_CONFIG_FOUND)
  pkg_check_modules(SOCKETCAN libsocketcan)
endif()

if(PKG_CONFIG_FOUND AND SOCKETCAN_FOUND)
  set(SOCKETCAN_LIB ${SOCKETCAN_LIBRARIES})
  set(SOCKETCAN_INCLUDE_DIRS ${SOCKETCAN_INCLUDE_DIRS})
  # Set CMake variable so autoconfig.h is generated with the correct value
  set(CSP_HAVE_LIBSOCKETCAN 1)
  # Also set for CSP library
  set(LIBSOCKETCAN_FOUND TRUE)
  set(LIBSOCKETCAN_LIBRARIES ${SOCKETCAN_LIBRARIES})
  set(LIBSOCKETCAN_INCLUDE_DIRS ${SOCKETCAN_INCLUDE_DIRS})
else()
  find_library(SOCKETCAN_LIB
    NAMES socketcan
    PATHS
      /usr/lib/x86_64-linux-gnu
      /usr/local/lib/x86_64-linux-gnu
      /usr/lib/arm-linux-gnueabihf
      /usr/local/lib/arm-linux-gnueabihf
      /usr/arm-linux-gnueabihf/usr/lib/arm-linux-gnueabihf
      /usr/arm-linux-gnueabihf/usr/lib
      /usr/arm-linux-gnueabihf/lib
      /usr/lib
      /usr/local/lib
  )
  if(NOT SOCKETCAN_LIB)
    message(FATAL_ERROR "libsocketcan not found")
  endif()
  set(SOCKETCAN_INCLUDE_DIRS
      /usr/include
      /usr/arm-linux-gnueabihf/usr/include)
  # Set CMake variable so autoconfig.h is generated with the correct value
  set(CSP_HAVE_LIBSOCKETCAN 1)
  # Also set for CSP library
  set(LIBSOCKETCAN_FOUND TRUE)
  set(LIBSOCKETCAN_LIBRARIES ${SOCKETCAN_LIB})
  set(LIBSOCKETCAN_INCLUDE_DIRS ${SOCKETCAN_INCLUDE_DIRS})
endif()

add_subdirectory(fsw/libcsp)

target_include_directories(csp PUBLIC ${SOCKETCAN_INCLUDE_DIRS})
target_compile_definitions(csp PUBLIC
  ENABLE_CAN=1
  ENABLE_CAN_SOCKETCAN=1
  CSP_ENABLE_SOCKETCAN=1
)
target_link_libraries(csp PUBLIC ${SOCKETCAN_LIB})

add_cfe_app(bus_comms fsw/src/bus_comms_app.c)

target_include_directories(bus_comms PUBLIC
  fsw/mission_inc
  fsw/platform_inc
  fsw/libcsp/include
  ${SOCKETCAN_INCLUDE_DIRS}
)

find_package(Threads REQUIRED)

# Plain signature to match mission build usage elsewhere
target_link_libraries(bus_comms
  csp
  Threads::Threads
  rt
  ${SOCKETCAN_LIB}
)

# Don't link driver_can directly into bus_comms - it's already in libcsp.so
# Linking it directly causes duplicate symbols and symbol resolution issues.
# bus_comms should use the symbols from libcsp.so via the csp target link.
# if(TARGET driver_can)
#   target_link_libraries(bus_comms driver_can)
# endif()
# if(TARGET if_can_socketcan)
#   target_link_libraries(bus_comms if_can_socketcan)
# endif()

set_target_properties(bus_comms PROPERTIES
  BUILD_RPATH "\$ORIGIN"
  INSTALL_RPATH "\$ORIGIN:\$ORIGIN/../../lib"
)

target_compile_options(bus_comms PRIVATE -Wno-error=pedantic)
target_compile_options(csp PRIVATE
  -Wno-error=pedantic
  -include ${CMAKE_CURRENT_SOURCE_DIR}/fsw/libcsp/libcsp_cfs_compat.h
)
if(TARGET driver_can)
  target_compile_options(driver_can PRIVATE
    -Wno-error=pedantic
    -include ${CMAKE_CURRENT_SOURCE_DIR}/fsw/libcsp/libcsp_cfs_compat.h
  )
endif()
if(TARGET if_zmq)
  target_compile_options(if_zmq PRIVATE
    -Wno-error=pedantic
    -include ${CMAKE_CURRENT_SOURCE_DIR}/fsw/libcsp/libcsp_cfs_compat.h
  )
endif()

# Copy libcsp.so next to the app for $ORIGIN rpath
add_custom_command(TARGET bus_comms POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:csp> $<TARGET_FILE_DIR:bus_comms>
)

# Copy optional driver .so only if it is a SHARED/MODULE lib or executable
function(_maybe_copy_target tgt)
  if(TARGET ${tgt})
    get_target_property(_t ${tgt} TYPE)
    if(_t STREQUAL "SHARED_LIBRARY" OR _t STREQUAL "MODULE_LIBRARY" OR _t STREQUAL "EXECUTABLE")
      add_custom_command(TARGET bus_comms POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${tgt}> $<TARGET_FILE_DIR:bus_comms>)
    endif()
  endif()
endfunction()

_maybe_copy_target(driver_can)
_maybe_copy_target(if_can_socketcan)

# Install libcsp.so to the same directories as bus_comms.so
# This ensures $ORIGIN rpath can find it when dlopen() loads the library
# The cFE system installs apps to cpu1/cf and cpu2/cf (where INSTALL_SUBDIR is typically "cf")
# We install to both cpu1/cf and cpu2/cf to match where bus_comms gets installed
install(FILES $<TARGET_FILE:csp>
  DESTINATION cpu1/cf
  COMPONENT runtime
  OPTIONAL
)
install(FILES $<TARGET_FILE:csp>
  DESTINATION cpu2/cf
  COMPONENT runtime
  OPTIONAL
)

if (ENABLE_UNIT_TESTS)
endif()





