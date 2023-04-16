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
#include <numeric>      // std::accumulate

#include <stdlib.h>     

namespace ns3 {



// ==========================================AMAR ADD KORA ================================================>


double averageVectorCalcMy(std::vector<double> myvect)
{
    size_t foundsize = myvect.size();
    if(foundsize==0){
        return 0.0; 
    }
    double sum_vect = 0.0;

    for ( int i=0; i < (int) foundsize; i++)
    {
        sum_vect += myvect[i];
    }
    double avgvect1 = (1.0*sum_vect)/(1.0*foundsize); 
    return avgvect1;
}


// ==========================================AMAR ADD KORA ================================================>

double calculateZeta(std::vector<double> myvect)
{

    size_t foundsize = myvect.size();
    if(foundsize==0){
        return 0.0; 
    }
    
    double avgvect1 = averageVectorCalcMy(myvect); 

    double sum_dev = 0.0;
    for(int i=0; i < (int) foundsize; i++)
    {
        sum_dev += abs(myvect[i]-avgvect1);
    }

    double zeta1 = (1.0*sum_dev)/foundsize; 

    return zeta1; 

}



} // namespace ns3

#endif /* RTT_ESTIMATOR1_H */
