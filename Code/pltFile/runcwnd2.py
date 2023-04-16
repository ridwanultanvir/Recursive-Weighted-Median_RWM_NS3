# Log output parser. For retreving RTT mean error values for one run
import sys
import os
import re

pathstr = "/home/tanvir/Desktop/ns3start1/ns-allinone-3.35/ns-3.35/outputFile/rttexperiment6.cwnd"

fdata = []
with open (pathstr) as file:
    fdata = file.readlines()


# print(fdata)
totalCwnd = 0
numLine = 0

for line in fdata:
    # print(line)
    x = line.split()
    cwnd1 = x[0]    
    cwnd1 = float(cwnd1)
    totalCwnd += (cwnd1)
    numLine += 1

if numLine > 0:
    avgcwnd = totalCwnd/numLine
    print ("avgcwnd: ", avgcwnd , " Num of data: ", numLine)

    nFlow1 = sys.argv[1]

    print(str(nFlow1) + " " + str(avgcwnd)+"\n")



    file1 = open("cwndrun2.txt", "a")
    #   why "a"
    #   "a" - Append - will append to the end of the file
    file1.write(str(nFlow1) + " " + str(avgcwnd)+"\n")


else:
    print("cwnd")

