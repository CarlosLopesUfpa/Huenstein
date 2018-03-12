##!/usr/bin/gnuplot
#
# Creates a version of a plot, which looks nice for inclusion on web pages
#
# AUTHOR: Hagen Wierstorf

reset

### Start multiplot (2x2 layout)


# --- GRAPH a




set terminal pngcairo size 800,600 enhanced font 'Verdana,9'
set output 'multiplot/Throughput.png'
set multiplot layout 2,2 rowsfirst
set label 1 'a' at graph 0.92,0.9 font ',8'

set title "Throughput - Scenario 1"

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# color definitions
set style line 1 lc rgb '#9FA2A2' pt 1 ps 1 lt 1 lw 1 # --- gray
set style line 2 lc rgb '#38BEF7' pt 2 ps 1 lt 1 lw 1 # --- blue

set key above right

set xlabel 'Time (s)'
set ylabel 'Throughput (s)'
#set xrange [0:100]
#set yrange [0:1]

plot 'twolines/Throughput_1.txt' u 1:2 t 'LTE Basic' w lp ls 1, \
     '' u 1:3 t 'Proposal Scenario' w lp ls 2





set label 1 'b' at graph 0.92,0.9 font ',8'
set title "Throughput - Scenario 2"

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# color definitions
set style line 1 lc rgb '#9FA2A2' pt 1 ps 1 lt 1 lw 1 # --- gray
set style line 2 lc rgb '#38BEF7' pt 2 ps 1 lt 1 lw 1 # --- blue


set key above right

set xlabel 'Time (s)'
set ylabel 'Throughput (s)'
#set xrange [0:100]
#set yrange [0:1]

plot 'twolines/Throughput_2.txt' u 1:2 t 'LTE Basic' w lp ls 1, \
     '' u 1:3 t 'Proposal Scenario' w lp ls 2




set label 1 'c' at graph 0.92,0.9 font ',8'
set title "Throughput - Scenario 3"

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# color definitions
set style line 1 lc rgb '#9FA2A2' pt 1 ps 1 lt 1 lw 1 # --- gray
set style line 2 lc rgb '#38BEF7' pt 2 ps 1 lt 1 lw 1 # --- blue


set key above right

set xlabel 'Time (s)'
set ylabel 'Throughput (s)'
#set xrange [0:100]
#set yrange [0:1]

plot 'twolines/Throughput_3.txt' u 1:2 t 'LTE Basic' w lp ls 1, \
     '' u 1:3 t 'Proposal Scenario' w lp ls 2






set label 1 'd' at graph 0.92,0.9 font ',8'
set title "Throughput - Scenario 4"

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# color definitions
set style line 1 lc rgb '#9FA2A2' pt 1 ps 1 lt 1 lw 1 # --- gray
set style line 2 lc rgb '#38BEF7' pt 2 ps 1 lt 1 lw 1 # --- blue


set key above right

set xlabel 'Time (s)'
set ylabel 'Throughput (s)'
#set xrange [0:100]
#set yrange [0:1]

plot 'twolines/Throughput_4.txt' u 1:2 t 'LTE Basic' w lp ls 1, \
     '' u 1:3 t 'Proposal Scenario' w lp ls 2


unset multiplot