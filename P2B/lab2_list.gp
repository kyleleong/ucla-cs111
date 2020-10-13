#! /usr/bin/gnuplot

set terminal png
set datafile separator ","

# Mutex vs. spin-lock synchronization
set title "2B 1: Throughput vs. Threads of Synchronization Types"
set xlabel "Threads"
set logscale x 2
set ylabel "Throughput (ops/sec)"
set logscale y
set output 'lab2b_1.png'
set key left top

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/$7) \
     title 'List with mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/$7) \
     title 'List with spin-lock' with linespoints lc rgb 'red'

# Wait-for-lock time and average time per operations against number of threads.
set title "2B 2: Wait-for-lock and Operation Time vs. Number of Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Time (ns)"
set logscale y
set output 'lab2b_2.png'
set key left top

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
     title 'Wait-for-lock' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
     title 'Time per operation' with linespoints lc rgb 'red'

# Successful number of iterations versus number of threads for 4 lists.
set title "2B 3: Iterations vs. Threads for 4 Lists"
set xlabel "Threads"
set logscale x 2
set ylabel "Iterations"
set logscale y
set output 'lab2b_3.png'
set key left top

plot \
     "< grep -e 'list-id-none,[0-9]*,' lab2b_list.csv" using ($2):($3) \
     title 'Unprotected w/ id' with points lc rgb 'red', \
     "< grep -e 'list-id-s,[0-9]*,' lab2b_list.csv" using ($2):($3) \
     title 'Spin-lock w/ id' with points lc rgb 'blue', \
     "< grep -e 'list-id-m,[0-9]*,' lab2b_list.csv" using ($2):($3) \
     title 'Mutex w/ id' with points lc rgb 'green'

# Proof that partitioned lists implementation works for Mutex
set title "2B 4: Partitioned List Performance vs. Mutex Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y
set output 'lab2b_4.png'
set key left top

plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 List' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 Lists' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 Lists' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 Lists'with linespoints lc rgb 'yellow'

# Proof that partitioned lists implementation works for spin lock
set title "2B 4: Partitioned List Performance vs. Spin-lock Threads"
set xlabel "Threads"
set logscale x 2
set ylabel "Operations per Second"
set logscale y
set output 'lab2b_5.png'
set key left top

plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '1 List' with linespoints lc rgb 'red', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '4 Lists' with linespoints lc rgb 'green', \
     "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '8 Lists' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
     title '16 Lists' with linespoints lc rgb 'yellow'