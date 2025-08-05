project(CFE_COMMS_APP C)

add_cfe_app(comms_app fsw/src/comms_app.c)
add_cfe_app_dependency(comms_app comms_lib)
add_cfe_tables(comms_app fsw/tables/comms_app_tbl.c)

target_include_directories(comms_app PUBLIC
  fsw/mission_inc
  fsw/platform_inc
  /usr/local/include  # CSP headers
)

# Explicitly find libcsp.so
find_library(CSP_LIBRARY csp PATHS /usr/local/lib NO_DEFAULT_PATH)

# If found, link against it
if (CSP_LIBRARY)
  target_link_libraries(comms_app ${CSP_LIBRARY})
else()
  message(FATAL_ERROR "libcsp.so not found in /usr/local/lib")
endif()


target_link_libraries(comms_app csp)  

if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif ()


