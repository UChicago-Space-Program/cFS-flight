project(CFE_COMMS_APP C)

add_cfe_app(comms_app fsw/src/comms_app.c)
add_cfe_app_dependency(comms_app comms_lib)
add_cfe_tables(comms_app fsw/tables/comms_app_tbl.c)

target_include_directories(comms_app PUBLIC
  fsw/mission_inc
  fsw/platform_inc
  /usr/local/include  # CSP headers
)

target_link_libraries(comms_app csp)  # ← FIXED: plain signature

if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif ()


