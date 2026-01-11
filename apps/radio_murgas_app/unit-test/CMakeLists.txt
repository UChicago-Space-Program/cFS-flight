##################################################################
#
# Coverage Unit Test build recipe
#
# This CMake file contains the recipe for building the radio_app unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

#
#
# NOTE on the subdirectory structures here:
#
# - "inc" provides local header files shared between the coveragetest,
#    wrappers, and overrides source code units
# - "coveragetest" contains source code for the actual unit test cases
#    The primary objective is to get line/path coverage on the FSW 
#    code units.
#
 
# Use the UT assert public API, and allow direct
# inclusion of source files that are normally private
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)


# Add a coverage test executable called "radio_murgas_app-ALL" that 
# covers all of the functions in radio_app.  
#
# Also note in a more complex app/lib the coverage test can also
# be broken down into smaller units (in which case one should use
# a unique suffix other than "ALL" for each unit).  For example,
# OSAL implements a separate coverage test per source unit.
add_cfe_coverage_test(radio_murgas_app ALL 
    "coveragetest/coveragetest_radio_app.c"
    "${CFE_RADIO_APP_SOURCE_DIR}/fsw/src/radio_app.c"
)
