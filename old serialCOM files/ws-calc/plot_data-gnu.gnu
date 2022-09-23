cd "/home/berkhme/vscode repo/SchoolRelated/old serialCOM files/ws-calc/"



set title "data plotting test: a:0 b:-90 [degrees]"
set xlabel "x -axis"
set ylabel "y -axis"
set zlabel "z -axis"


set xrange [-250:250]
set yrange [0:250]
# set zrange [0:400]


# set contour
set view 0, 0, 1.1, 1
# set dgrid3d
set grid
set cntrparam

# set dgrid3d 30, 30

set style fill transparent #0.1
# set style fill solid



scalar = 1.1

#u 1:2:3 w points lt 1 pt 10 ps variable #with circles palette
# plot "testResult/test2/basicTest_2_0.-45_result.dat" u 1:2:(abs($4-$2)*scalar) with circles title "error value: mean value", \
     "testResult/test2/basicTest_2_0.-45_result.dat" u 1:4 with points ls 7

plot "testResult/test2/basicTest_2_0.-90_result.dat" u 1:2:($3-$1):($4-$2) with vectors nohead ls 8 title "error value: mean value of x and y", \
     "testResult/test2/basicTest_2_0.-90_result.dat" u 1:2 with points ls 7 title "given position", \
     "testResult/test2/basicTest_2_0.-90_result.dat" u 3:4 with points ls 4 title "read position"

# plot "testResult/test2/basicTest_2_0.-90_result.dat" u 1:2:(sqrt(($3-$1)**2+($4-$2)**2)*scalar) with circles palette title "error value: mean value of x and y", \
     # "testResult/test2/basicTest_2_0.-90_result.dat" u 3:4 with points ls 7 title "read position", \
    # "testResult/test2/basicTest_2.dat" u 1:2 with points