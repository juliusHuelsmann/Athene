

cmake_minimum_required (VERSION 2.8)
project ( gtd.out )

 
#
# OpenCV Packaage
find_package( OpenCV REQUIRED )

 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")


# add the executable
# and link OpenCV Lib.
add_executable(gtd.out generateTrainingData.cpp ../Utils/Utils.h ../Utils/Utils.cpp ../Utils/Constants.h)
target_link_libraries( gtd.out ${OpenCV_LIBS})


