cmake_minimum_required(VERSION 3.16)
project(CFS_RADIO_APP C)

# Create the app module
add_cfe_app(radio_app fsw/src/radio_app.c)

# Include the public API from sample_lib to demonstrate how
# to call library-provided functions
add_cfe_app_dependency(radio_app sample_lib)

# Add table
add_cfe_tables(radio_app fsw/tables/radio_app_tbl.c)

target_include_directories(radio_app PUBLIC
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
