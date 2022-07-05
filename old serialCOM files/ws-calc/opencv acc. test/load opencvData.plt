
set title "data plotting test"
set xlabel "x -axis"
set ylabel "y -axis"
set zlabel "z -axis"


set xrange [-400:400]
set yrange [0:400]
set zrange [0:540]


# set contour
set view 0, 0, 1.1, 1
# set dgrid3d
set grid
set cntrparam

# set dgrid3d 30, 30
set ticslevel 0


# splot full dot list
cd 'C:\Users\aa82637\source\repos\Arduino serial Communication test\Arduino serial Communication test\ws-calc\opencv acc. test'
splot "res10_z216.dat", \
"z216_0.0.dat", \
"z216_0.45.dat", \
"z216_0.-45.dat", \
"z216_45.0.dat", \
"z216_-45.0.dat", \

