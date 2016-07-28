

cmake_minimum_required (VERSION 2.8)
project ( preparation.out )

 
#
# OpenCV Packaage
find_package( OpenCV REQUIRED )

 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")


# add the executable
# and link OpenCV Lib.
add_executable(preparation.out Testmain.cpp CharPreparationSpace.h ../../Utils/Constants.h CharPreparationSpace.cpp ../../Utils/Utils.h ../../Utils/Utils.cpp)
target_link_libraries( preparation.out ${OpenCV_LIBS})


