cd "/home/berkhme/vscode repo/SchoolRelated/old serialCOM files/ws-calc/"


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


splot "testResult/basicTest_1.dat" u 1:2:3 with lines palette
