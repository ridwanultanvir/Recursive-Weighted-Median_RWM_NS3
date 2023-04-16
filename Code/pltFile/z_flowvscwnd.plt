set terminal png size 1280, 800
set output "flowvscwnd.png"
set title "Flow Vs CWND"
set xlabel "Number of TCP Flow"
set ylabel "CWND (kB) kbytes"
plot "cwnd1.txt" using 1:2 with linespoints title "CWND Jacobson" lw 2.5 linecolor  rgb 'blue', \
     "cwnd2.txt" using 1:2 with linespoints title "CWND Weighted Median" lw 2.5 linecolor  rgb 'red'