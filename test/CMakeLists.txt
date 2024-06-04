project(Google_Tests)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# adding the Google_Tests_run target
add_executable(CPUSimulator_Tests
        ${SOURCE_FILES}

        ../source/CPU32/CPU32.cpp
)

target_link_libraries(CPUSimulator_Tests gtest gtest_main)

#
##
#
########
#
##
#

###################
#					#
#    Google Test	#
#					#
###################



include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#
##
#
########
#
##
#