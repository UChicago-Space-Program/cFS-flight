project(CFE_COMMS_APP C)

# Create the app module
add_cfe_app(comms_app fsw/src/comms_app.c)

# Include the public API from comms_lib to demonstrate how
# to call library-provided functions
add_cfe_app_dependency(comms_app comms_lib)

# Add table
add_cfe_tables(comms_app fsw/tables/comms_app_tbl.c)

target_include_directories(comms_app PUBLIC
  fsw/mission_inc
  fsw/platform_inc
)

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make 
# direct function calls into this application.
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
