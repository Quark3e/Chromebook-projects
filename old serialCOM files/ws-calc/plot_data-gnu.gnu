cd "/home/berkhme/vscode repo/SchoolRelated/old serialCOM files/ws-calc/"


set title "data plotting test"
set xlabel "x -axis"
set ylabel "y -axis"
set zlabel "z -axis"


set xrange [-400:400]
set yrange [0:400]
set zrange [0:400]


# set contour
set view 0, 0, 1.1, 1
# set dgrid3d
set grid
set cntrparam

# set dgrid3d 30, 30

set style fill transparent #0.5

scalar = 1

#u 1:2:3 w points lt 1 pt 10 ps variable #with circles palette

plot "testResult/basicTest_1_result.dat" u 1:2:(abs(abs($1-$3)-abs($2-$4))*scalar) with circles title "error value: uses both x and y"#, \
# "testResult/basicTest_1_result.dat" u 1:2:4 with circles