
## Retransmission_Timeout_Weighted_RTW_NS3

[RTW_Report_NS3.pdf](https://github.com/ridwanultanvir/Retransmission_Timeout_Weighted_RTW_NS3/blob/master/Report/RTW_Report_NS3.pdf)

- The proposed RTO algorithm utilizing recursive weighted median (RWM) filters yields significantly tighter RTT bounds than Jacobson's algorithm over Internet traffic with heavy tailed statistics.
- The RWM filters are more effective in impulsive signal environments, which is often the case with RTT signals with heavy tailed statistics. This is due to the fact that linear filters, such as Jacobson's algorithm, are adequate for estimation in Gaussian signal environments but not as effective in impulsive signal environments.


## Comparison of RTW and Jacobson's Algorithm






### CWND 




<p align="center">
  <img src="images/1flowvscwnd.png" alt="CWND" />
  <br />
  <em>Fig: CWND graph</em>
</p>


<!-- ![CWND graph](images/1flowvscwnd.png)
*FLow vs CWND graph* -->


### MEAN RTT Error 

<!-- ![MEAN RTT Error Graph](images/2flowvsmeanrtterror.png)
*FLow vs MEAN RTT Error* -->
<p align="center">
  <img src="images/2flowvsmeanrtterror.png" alt="MEAN RTT Error Graph" />
  <br />
  <em>Fig: MEAN RTT Error Graph</em>
</p>


### RTT RTO Plot

<!-- ![RTT RTO Plot Jacobson](images/b1_rtt_rto_plot-d2-jacob.png)
*RTT RTO Plot Jacobson*


![RTT RTO Plot weighted RTW](images/b2_rtt_rto_plot-d2-weighted.png)
*RTT RTO Plot weighted RTW* -->

<p align="center">
  <img src="images/b1_rtt_rto_plot-d2-jacob.png" alt="RTT RTO Plot Jacobson" />
  <br />
  <em>Fig: RTT RTO Plot Jacobson</em>
</p>


<p align="center">
  <img src="images/b2_rtt_rto_plot-d2-weighted.png" alt="RTT RTO Plot weighted RTW" />
  <br />
  <em>Fig: RTT RTO Plot weighted RTW</em>
</p>
