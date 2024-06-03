
g++ \
-DIMGUI_USER_CONFIG=\"/usr/local/include/imgui/examples/example_allegro5/imconfig_allegro5.h\" \
-I/usr/local/include/imgui/ \
main.cpp \
/usr/local/include/imgui/backends/imgui_impl_allegro5.cpp \
/usr/local/include/imgui/imgui*.cpp \
-lallegro \
-lallegro_main \
-lallegro_primitives \
-o EXE_local

./EXE_local
