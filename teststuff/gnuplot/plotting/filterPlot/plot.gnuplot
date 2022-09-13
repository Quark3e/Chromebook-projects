# Set linestyle 1 to blue (#0060ad)
set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

set xrange [0:50]

set style line 1 lt rgb "blue" lw 3 pt 6

plot 'filter_9-1.dat' with linespoints linestyle 6 title "9-1", \
    'filter_8-2.dat' with linespoints linestyle 6 title "8-2", \
    'filter_7-3.dat' with linespoints linestyle 6 title "7-3", \
    'filter_6-4.dat' with linespoints linestyle 6 title "6-4"