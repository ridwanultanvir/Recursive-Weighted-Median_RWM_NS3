

# FOR RUN CHECK 
# declare -a arr=("100" "90" "60" "40")

# declare -a arr=("30" "60")

declare -a arr=("3" "7" "10" "17" "20" "25" "30" "34" "40" "45" "50" "60" "68" "75" "80" "85" "90" "100" "110" "120" "130")





for i in "${arr[@]}"
do
   echo "$i"
   ./waf --run "scratch/rttexperiment6 --nFlow=$i"
   python3 runcwnd2.py "$i"
   # or do whatever with individual element of the array
done


# python3 merror.py
