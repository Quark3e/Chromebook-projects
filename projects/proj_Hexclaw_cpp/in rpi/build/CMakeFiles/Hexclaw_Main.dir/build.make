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
CMAKE_SOURCE_DIR = "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build"

# Include any dependencies generated for this target.
include CMakeFiles/Hexclaw_Main.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Hexclaw_Main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Hexclaw_Main.dir/flags.make

CMakeFiles/Hexclaw_Main.dir/main.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Hexclaw_Main.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/main.cpp.o -c "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/main.cpp"

CMakeFiles/Hexclaw_Main.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/main.cpp" > CMakeFiles/Hexclaw_Main.dir/main.cpp.i

CMakeFiles/Hexclaw_Main.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/main.cpp" -o CMakeFiles/Hexclaw_Main.dir/main.cpp.s

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.o: /usr/local/include/imgui/backends/imgui_impl_allegro5.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.o -c /usr/local/include/imgui/backends/imgui_impl_allegro5.cpp

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/include/imgui/backends/imgui_impl_allegro5.cpp > CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.i

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/include/imgui/backends/imgui_impl_allegro5.cpp -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.s

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.o: /usr/local/include/imgui/imgui_demo.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.o -c /usr/local/include/imgui/imgui_demo.cpp

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/include/imgui/imgui_demo.cpp > CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.i

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/include/imgui/imgui_demo.cpp -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.s

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.o: /usr/local/include/imgui/imgui_draw.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.o -c /usr/local/include/imgui/imgui_draw.cpp

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/include/imgui/imgui_draw.cpp > CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.i

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/include/imgui/imgui_draw.cpp -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.s

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.o: /usr/local/include/imgui/imgui_tables.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.o -c /usr/local/include/imgui/imgui_tables.cpp

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/include/imgui/imgui_tables.cpp > CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.i

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/include/imgui/imgui_tables.cpp -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.s

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.o: /usr/local/include/imgui/imgui_widgets.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.o -c /usr/local/include/imgui/imgui_widgets.cpp

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/include/imgui/imgui_widgets.cpp > CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.i

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/include/imgui/imgui_widgets.cpp -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.s

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.o: CMakeFiles/Hexclaw_Main.dir/flags.make
CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.o: /usr/local/include/imgui/imgui.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.o -c /usr/local/include/imgui/imgui.cpp

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/include/imgui/imgui.cpp > CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.i

CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/include/imgui/imgui.cpp -o CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.s

# Object files for target Hexclaw_Main
Hexclaw_Main_OBJECTS = \
"CMakeFiles/Hexclaw_Main.dir/main.cpp.o" \
"CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.o" \
"CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.o" \
"CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.o" \
"CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.o" \
"CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.o" \
"CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.o"

# External object files for target Hexclaw_Main
Hexclaw_Main_EXTERNAL_OBJECTS =

Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/main.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_demo.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_draw.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_tables.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui_widgets.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/usr/local/include/imgui/imgui.cpp.o
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/build.make
Hexclaw_Main: /usr/local/lib/static/libPiPCA9685.a
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_gapi.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_stitching.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_aruco.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_barcode.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_bgsegm.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_bioinspired.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_ccalib.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn_objdetect.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn_superres.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_dpm.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_face.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_freetype.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_fuzzy.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_hdf.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_hfs.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_img_hash.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_intensity_transform.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_line_descriptor.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_mcc.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_quality.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_rapid.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_reg.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_rgbd.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_saliency.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_stereo.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_structured_light.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_superres.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_surface_matching.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_tracking.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_videostab.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_wechat_qrcode.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_xfeatures2d.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_xobjdetect.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_xphoto.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_shape.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_highgui.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_datasets.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_plot.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_text.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_ml.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_phase_unwrapping.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_optflow.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_ximgproc.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_video.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_videoio.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_imgcodecs.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_objdetect.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_calib3d.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_features2d.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_flann.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_photo.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_imgproc.so.4.5.5
Hexclaw_Main: /usr/local/lib/arm-linux-gnueabihf/libopencv_core.so.4.5.5
Hexclaw_Main: CMakeFiles/Hexclaw_Main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX executable Hexclaw_Main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Hexclaw_Main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Hexclaw_Main.dir/build: Hexclaw_Main

.PHONY : CMakeFiles/Hexclaw_Main.dir/build

CMakeFiles/Hexclaw_Main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Hexclaw_Main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Hexclaw_Main.dir/clean

CMakeFiles/Hexclaw_Main.dir/depend:
	cd "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi" "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi" "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build" "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build" "/home/pi/Chromebook-projects/projects/proj_Hexclaw_cpp/in rpi/build/CMakeFiles/Hexclaw_Main.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/Hexclaw_Main.dir/depend
