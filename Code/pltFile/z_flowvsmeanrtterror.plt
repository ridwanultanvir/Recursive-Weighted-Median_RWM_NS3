set terminal png size 1280, 800
set output "flowvsmeanrtterror.png"
set title "Flow Vs Mean RTT Error"
set xlabel "Number of TCP Flow"
set ylabel "Mean RTT Error(ms)"
plot "merror1.txt" using 1:2 with linespoints title "Mean RTT Error Jacobson" lw 2.5 linecolor  rgb 'blue', \
     "merror2.txt" using 1:2 with linespoints title "Mean RTT Error Weighted Median" lw 2.5 linecolor  rgb 'red'