/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
//
// Copyright (c) 2006 Georgia Tech Research Corporation
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation;
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//



#ifndef RTT_ESTIMATOR1_H
#define RTT_ESTIMATOR1_H

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/assert.h"
#include <algorithm>
#include <cmath>
//cmath: pow() function 

/*

LINKS: 
1. https://stackoverflow.com/questions/2114797/compute-median-of-values-stored-in-vector-c
2. http://cplusplus.com/forum/beginner/193204/                 ==> 1,2 both same code 

3. https://stackoverflow.com/questions/14259993/namespace-std-has-no-member-sort

Khubi crucial error; 
std::sort ==> this will give error 

sort(myvect.begin(), myvect.end()); ===> also error 

must include ```#include <algorithm>```
*/ 

namespace ns3 {


/*
get the values of constant ; 
ACCORDING TO THE PAPER 
A1 = β = 1/2   ------(2)
Bj = α^(j−1) where α = 7/8 -------------(3)
*/ 

double getConstantA1()
{
  //beta1= 0.5 
  double const_a1 = 0.5; 
  return const_a1; 
}

// Bj = α^(j−1)
double getSpecificConstantBj(int indexNum)
{
  double alpha1 = (1.0*7)/(1.0*8); //focus this alpha1 is NOT THE SAME AS JACOBSON alpha 
  int j = indexNum; 
  // pow(double base, double exponent);
  double b_j = pow(alpha1, j-1); 
  // pow(2, ((i - m_numExperts)

  return b_j; 

}

/*
ACCORDING TO THE PAPER: M = 5; 
so vector size will be 5 

*/
std::vector<double> getConstantVectorBj()
{
  int sizeV = 5; 
  double alpha1 = (1.0*7)/(1.0*8); //focus this alpha1 is NOT THE SAME AS JACOBSON alpha 

  // pow(double base, double exponent);

  std::vector<double> bj_vector;
  //ns3 gives error if I give vector a size
  //so I have to push_back; tai push_back korsi 
  for(int i=0; i<sizeV; i++){
    double bj_val = pow(alpha1, i-1); 
    bj_vector.push_back(bj_val); 

  }

  return bj_vector; 


}

std::vector<double> getConstantVectorBj(int sizeV)
{
  // int sizeV = 5; 
  double alpha1 = (1.0*7)/(1.0*8); //focus this alpha1 is NOT THE SAME AS JACOBSON alpha 

  // pow(double base, double exponent);

  std::vector<double> bj_vector;
  //ns3 gives error if I give vector a size
  //so I have to push_back; tai push_back korsi 
  for(int i=0; i<sizeV; i++){
    double bj_val = pow(alpha1, i-1); 
    bj_vector.push_back(bj_val); 

  }

  return bj_vector; 


}



double medianVectorCalcMy(std::vector<double> myvect)
{
  size_t foundsize = myvect.size();

  if (foundsize == 0)
  {
    //actually vector e kisui nai 
    return 0;  
    
  }
  else
  {
    
    sort(myvect.begin(), myvect.end());
    if (foundsize % 2 == 0)
    {
        double median1 = 1.0* (myvect[foundsize / 2 - 1] + myvect[foundsize / 2]); 
        median1 = median1/2.0; 
        return  median1;
    }
    else 
    {
        double median1 = 1.0* (myvect[foundsize / 2]); 
        return  median1;
    }
  }
}


} // namespace ns3

#endif /* RTT_ESTIMATOR1_H */
