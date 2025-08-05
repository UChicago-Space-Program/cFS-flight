project(CFE_COMMS_APP C)

add_cfe_app(comms_app fsw/src/comms_app.c)
add_cfe_app_dependency(comms_app comms_lib)
add_cfe_tables(comms_app fsw/tables/comms_app_tbl.c)

# Include CSP headers
target_include_directories(comms_app PUBLIC
  fsw/mission_inc
  fsw/platform_inc
  /usr/local/include  # CSP headers
)

# Explicitly find and link against the shared CSP library
find_library(CSP_LIBRARY NAMES csp PATHS /usr/local/lib NO_DEFAULT_PATH)

if (NOT CSP_LIBRARY)
  message(FATAL_ERROR "libcsp.so not found in /usr/local/lib — ensure CSP is installed and built as a shared library")
endif()

target_link_libraries(comms_app ${CSP_LIBRARY})

# Optional: add unit test subdirectory
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif()



