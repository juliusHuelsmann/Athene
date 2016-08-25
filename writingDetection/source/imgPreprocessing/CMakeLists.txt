

cmake_minimum_required (VERSION 2.8)
project ( imgpre.out )

 
#
# OpenCV Packaage
find_package( OpenCV REQUIRED )

 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")


# add the executable
# and link OpenCV Lib.
add_executable(imgpre.out main.cpp preprocessing.h preprocessing.cpp ../Utils/Constants.h ../Utils/Utils.h ../Utils/Utils.cpp)
target_link_libraries( imgpre.out ${OpenCV_LIBS})


