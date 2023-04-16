set terminal png size 1280, 800
set output "rtt_rto_plot-d2-weighted.png"
set title "Time Vs RTT; RTO"
set xlabel "Time"
set ylabel "RTT"
plot "d2-weighted-rtt.data" using 1:2 with lines title "RTT", \
     "d2-weighted-rto.data" using 1:2 with lines title "RTO"
