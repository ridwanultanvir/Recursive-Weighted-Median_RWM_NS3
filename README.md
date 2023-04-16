
## RWM using Network Simulator 3 (NS3)

[Report_Recursive-Weighted-Median_NS3.pdf](https://github.com/ridwanultanvir/Recursive-Weighted-Median_RWM_NS3/blob/master/Report/Report_Recursive-Weighted-Median_NS3.pdf)

- The proposed RTO algorithm utilizing recursive weighted median (RWM) filters yields significantly tighter RTT bounds than Jacobson's algorithm over Internet traffic with heavy tailed statistics.
- The RWM filters are more effective in impulsive signal environments, which is often the case with RTT signals with heavy tailed statistics. This is due to the fact that linear filters, such as Jacobson's algorithm, are adequate for estimation in Gaussian signal environments but not as effective in impulsive signal environments.


## Comparison of RWM and Jacobson's Algorithm






### CWND 




<p align="center">
  <img src="images/1flowvscwnd.png" alt="CWND" />
  <br />
  <em>Fig: CWND graph</em>
</p>



### MEAN RTT Error 

<p align="center">
  <img src="images/2flowvsmeanrtterror.png" alt="MEAN RTT Error Graph" />
  <br />
  <em>Fig: MEAN RTT Error Graph</em>
</p>


### RTT RTO Plot


<p align="center">
  <img src="images/b1_rtt_rto_plot-d2-jacob.png" alt="RTT RTO Plot Jacobson" />
  <br />
  <em>Fig: RTT RTO Plot Jacobson</em>
</p>


<p align="center">
  <img src="images/b2_rtt_rto_plot-d2-weighted.png" alt="RTT RTO Plot weighted RWM" />
  <br />
  <em>Fig: RTT RTO Plot weighted RWM</em>
</p>
