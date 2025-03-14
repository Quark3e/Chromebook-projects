# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build

# Include any dependencies generated for this target.
include CMakeFiles/cppEXE.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cppEXE.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cppEXE.dir/flags.make

CMakeFiles/cppEXE.dir/cpp_cameras.cpp.o: CMakeFiles/cppEXE.dir/flags.make
CMakeFiles/cppEXE.dir/cpp_cameras.cpp.o: ../cpp_cameras.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cppEXE.dir/cpp_cameras.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cppEXE.dir/cpp_cameras.cpp.o -c /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/cpp_cameras.cpp

CMakeFiles/cppEXE.dir/cpp_cameras.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cppEXE.dir/cpp_cameras.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/cpp_cameras.cpp > CMakeFiles/cppEXE.dir/cpp_cameras.cpp.i

CMakeFiles/cppEXE.dir/cpp_cameras.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cppEXE.dir/cpp_cameras.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/cpp_cameras.cpp -o CMakeFiles/cppEXE.dir/cpp_cameras.cpp.s

# Object files for target cppEXE
cppEXE_OBJECTS = \
"CMakeFiles/cppEXE.dir/cpp_cameras.cpp.o"

# External object files for target cppEXE
cppEXE_EXTERNAL_OBJECTS =

cppEXE: CMakeFiles/cppEXE.dir/cpp_cameras.cpp.o
cppEXE: CMakeFiles/cppEXE.dir/build.make
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_gapi.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_stitching.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_aruco.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_barcode.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_bgsegm.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_bioinspired.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_ccalib.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn_objdetect.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn_superres.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_dpm.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_face.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_freetype.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_fuzzy.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_hdf.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_hfs.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_img_hash.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_intensity_transform.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_line_descriptor.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_mcc.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_quality.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_rapid.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_reg.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_rgbd.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_saliency.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_stereo.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_structured_light.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_superres.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_surface_matching.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_tracking.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_videostab.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_wechat_qrcode.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_xfeatures2d.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_xobjdetect.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_xphoto.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_shape.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_highgui.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_datasets.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_plot.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_text.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_ml.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_phase_unwrapping.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_optflow.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_ximgproc.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_video.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_videoio.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_imgcodecs.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_objdetect.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_calib3d.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_features2d.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_flann.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_photo.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_imgproc.so.4.5.5
cppEXE: /usr/local/lib/arm-linux-gnueabihf/libopencv_core.so.4.5.5
cppEXE: CMakeFiles/cppEXE.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable cppEXE"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cppEXE.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cppEXE.dir/build: cppEXE

.PHONY : CMakeFiles/cppEXE.dir/build

CMakeFiles/cppEXE.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cppEXE.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cppEXE.dir/clean

CMakeFiles/cppEXE.dir/depend:
	cd /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build /home/pi/Chromebook-projects/science/math/analytic_geometry/camTest/build/CMakeFiles/cppEXE.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cppEXE.dir/depend

