##!/usr/bin/gnuplot
#
# Creates a version of a plot, which looks nice for inclusion on web pages
#
# AUTHOR: Hagen Wierstorf

reset


set terminal pngcairo size 800,600 enhanced font 'Verdana,11'
set output 'Delay_1_2.png'
set title "Delay - Scenario 1 and 2"
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
set style line 1 lc rgb '#ACCAE4' pt 2 ps 1 lt 1 lw 2 # --- gray
set style line 2 lc rgb '#ACCAE4' pt 10 ps 1 lt 1 lw 2 # --- gray 
set style line 3 lc rgb '#0690CB' pt 7 ps 1 lt 1 lw 2 # --- blue
set style line 4 lc rgb '#0690CB' pt 1 ps 1 lt 1 lw 2 # --- blue

# google colors 
# set style line 1 lc rgb '#EA4335' pt 2 ps 1 lt 1 lw 2 # --- gray
# set style line 2 lc rgb '#34A853' pt 10 ps 1 lt 1 lw 2 # --- gray 
# set style line 3 lc rgb '#4285F4' pt 7 ps 1 lt 1 lw 2 # --- blue
# set style line 4 lc rgb '#FBBC05' pt 1 ps 1 lt 1 lw 2 # --- blue

set key box above right # muda a posição da descrição das linhas

set xlabel 'Time (s)'
set ylabel 'Delay (s)'
#set xrange [0:100]
#set yrange [-14:5]

plot 'Delay_1_2.txt' u 1:2 t 'Basic lte Scenario 1' w lp ls 1, \
     '' u 1:3 t 'Basic lte Scenario 2' w lp ls 2, \
     '' u 1:4 t 'Proposal Scenario 1' w lp ls 3, \
     '' u 1:5 t 'Proposal Scenario 2' w lp ls 4