cd "/home/berkhme/Chromebook-projects/projects/proj_4-FullRpi/accuracy testing/"



set xlabel "x -axis"
set ylabel "y -axis"
set zlabel "z -axis"


set xrange [-250:250]
set yrange [0:250]
# set zrange [0:400]


# set contour
set view 50, 90, 1.1, 1
# set dgrid3d
set grid
set cntrparam

# set dgrid3d 30, 30

set style fill transparent #0.1
# set style fill solid


scalar = 1.1

alpha = "0"
beta = "-45"
setAngles = alpha.".".beta

set title "data plotting test: a:".alpha." b:".beta."[degrees]"

filename = "test2/basicTest_2_".setAngles."_Result"

# splot filename.".dat" u 1:2:3:($4-$1):($5-$2):($6-$3) with vectors nohead ls 8 title "error value: mean value of x and y", \
#      filename.".dat" u 1:2:3 with points ls 7 title "given position", \
#      filename.".dat" u 4:5:6 with points ls 4 title "read position"

plot filename.".dat" u 1:2:($4-$1):($5-$2) with vectors nohead ls 8 title "error value: mean value of x and y", \
     filename.".dat" u 1:2 with points ls 7 title "given position", \
     filename.".dat" u 4:5 with points ls 4 title "read position"
