set terminal png size 1280, 800
set output "rtt_rto_plot-d2-jacob.png"
set title "Time Vs RTT; RTO"
set xlabel "Time"
set ylabel "RTT"
plot "d2-jacob-rtt.data" using 1:2 with lines title "RTT", \
     "d2-jacob-rto.data" using 1:2 with lines title "RTO"
