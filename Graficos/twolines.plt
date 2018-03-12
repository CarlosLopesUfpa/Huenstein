##!/usr/bin/gnuplot
#
# Creates a version of a plot, which looks nice for inclusion on web pages
#
# AUTHOR: Hagen Wierstorf

reset


set terminal pngcairo size 800,600 enhanced font 'Verdana,9'
set output 'Delay_4.png'
set title "Delay - Scenario 3"
# svg
#set terminal svg size 410,250 fname 'Verdana, Helvetica, Arial, sans-serif' \
#fsize '9' rounded dashed
#set output 'nice_web_plot.svg'

# define axis
# remove border on top and right and set color to gray
set style line 11 lc rgb '#808080' lt 1
set border 3 back ls 11
set tics nomirror
# define grid
set style line 12 lc rgb '#808080' lt 0 lw 1
set grid back ls 12

# color definitions
set style line 1 lc rgb '#38BEF7' pt 1 ps 1 lt 1 lw 2 # --- blue
set style line 2 lc rgb '#CDD3D5' pt 6 ps 1 lt 1 lw 2 # --- gray

set key bottom right

set xlabel 'Time (s)'
set ylabel 'Delay (s)'
#set xrange [0:100]
#set yrange [0:1]

plot 'twolines/Delay_4.txt' u 1:2 t 'LTE Basic' w lp ls 1, \
     '' u 1:3 t 'Proposal Scenario' w lp ls 2

unset multiplot