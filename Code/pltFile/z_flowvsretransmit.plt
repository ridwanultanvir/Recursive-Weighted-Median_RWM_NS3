set terminal png size 1280, 800
set output "flowvsretransmit.png"
set title "Flow Vs Percentage of Retransmitted Packet"
set xlabel "Flow Number"
set ylabel "Percentage of Retransmitted Packet"
plot "retrans1.txt" using 1:2 with linespoints title "% Retransmitted Jacobson" lw 2.5 linecolor  rgb 'blue', \
     "retrans2.txt" using 1:2 with linespoints title "% Retransmitted Weighted Median" lw 2.5 linecolor  rgb 'red'
