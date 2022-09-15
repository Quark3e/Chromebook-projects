cd "/home/berkhme/vscode repo/SchoolRelated/old serialCOM files/ws-calc/ws - top z-value - alphaBeta"

set title "data plotting test"
set xlabel "x -axis"
set ylabel "y -axis"
set zlabel "z -axis"


set xrange [-400:400]
set yrange [-50:400]
set zrange [0:540]


# set contour
set view 90, 0, 1.1, 1
# set dgrid3d
set grid
set cntrparam

# set dgrid3d 30, 30


splot "0.-90-z_val.dat"
