project(CFE_ADCS_APP C)

# Create the app module
add_cfe_app(adcs_app fsw/src/adcs_app.c)

# Include the public API from adcs_lib to demonstrate how
# to call library-provided functions
#add_cfe_app_dependency(adcs_app adcs_lib)

# Add table
add_cfe_tables(adcs_app fsw/tables/adcs_app_tbl.c)

target_include_directories(adcs_app PUBLIC
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
