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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: Rajib Bhattacharjea<raj.b@gatech.edu>
//

// Ported from:
// Georgia Tech Network Simulator - Round Trip Time Estimator Class
// George F. Riley.  Georgia Tech, Spring 2002

// Base class allows variations of round trip time estimators to be
// implemented

#include <iostream>
#include <fstream>
#include <cmath>

#include "rtt-estimator.h"
#include "rtt-estimator1.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/log.h"

// eshob .txt file amar debug er shubhidhar jonno create korsi 

//File1 ; File4 : exactly 3 jaygay ase
static  std::ofstream MyRttFile1("rttFile1.txt");
static  std::ofstream MyRttFile4("rttFile4.txt");

//uporer 2ta .txt file amar mean error ber korte help korbe 


//egula jodi kon function e ; 
//eshob log print korte chao then kaje lagbe 
static  std::ofstream MyRttFile2("rttFile2.txt");
static  std::ofstream MyRttFile3("rttFile3.txt");
static  std::ofstream MyRttFile5("rttFile5.txt");
static  std::ofstream MyRttFile6("rttFile6.txt");

/*
1-3.txt FOR MEAN DEVIATION 
4-6.txt FOR WEIGHTED MEDIAN 


MAINLY kaje lage 1,4.txt

Kon function call hoy: egula lekha 2,5.txt te 

*/ 



namespace ns3 {



NS_LOG_COMPONENT_DEFINE ("RttEstimator");

NS_OBJECT_ENSURE_REGISTERED (RttEstimator);

/// Tolerance used to check reciprocal of two numbers.
static const double TOLERANCE = 1e-6;

TypeId 
RttEstimator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttEstimator")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
    .AddAttribute ("InitialEstimation", 
                   "Initial RTT estimate",
                   TimeValue (Seconds (1.0)),
                   MakeTimeAccessor (&RttEstimator::m_initialEstimatedRtt),
                   MakeTimeChecker ())
  ;
  return tid;
}

Time
RttEstimator::GetEstimate (void) const
{
  return m_estimatedRtt;
}

Time 
RttEstimator::GetVariation (void) const
{
  return m_estimatedVariation;
}


// Base class methods

RttEstimator::RttEstimator ()
  : m_nSamples (0)
{ 
  NS_LOG_FUNCTION (this);
  
  // We need attributes initialized here, not later, so use the 
  // ConstructSelf() technique documented in the manual
  ObjectBase::ConstructSelf (AttributeConstructionList ());
  m_estimatedRtt = m_initialEstimatedRtt;
  m_estimatedVariation = Time (0);
  NS_LOG_DEBUG ("Initialize m_estimatedRtt to " << m_estimatedRtt.GetSeconds () << " sec.");
  NS_LOG_DEBUG ("Initialize m_estimatedVariation to " << m_estimatedVariation.GetSeconds () << " sec.");
}

RttEstimator::RttEstimator (const RttEstimator& c)
  : Object (c),
    m_initialEstimatedRtt (c.m_initialEstimatedRtt),
    m_estimatedRtt (c.m_estimatedRtt),
    m_estimatedVariation (c.m_estimatedVariation),
    m_nSamples (c.m_nSamples)
{
  NS_LOG_FUNCTION (this);
}

RttEstimator::~RttEstimator ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
RttEstimator::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void RttEstimator::Reset ()
{ 
  NS_LOG_FUNCTION (this);
  // Reset to initial state
  m_estimatedRtt = m_initialEstimatedRtt;
  m_estimatedVariation = Time (0);
  m_nSamples = 0;
}

uint32_t 
RttEstimator::GetNSamples (void) const
{
  return m_nSamples;
}

//<======================================================================> 
// AMAR VECTOR gula return kortesi 
// tcp-socket-base e kaje lagbe egula 

// ==========================================AMAR ADD KORA ================================================>
std::vector<double>
RttEstimator::GetVectorRttEstimate (void) const
{
  return m_estimated_rtt_vector;
}

std::vector<double>
RttEstimator::GetVectorRttActual (void) const
{
  return m_actual_rtt_vector;
}




//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Mean-Deviation Estimator

NS_OBJECT_ENSURE_REGISTERED (RttMeanDeviation);

TypeId 
RttMeanDeviation::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttMeanDeviation")
    .SetParent<RttEstimator> ()
    .SetGroupName ("Internet")
    .AddConstructor<RttMeanDeviation> ()
    .AddAttribute ("Alpha",
                   "Gain used in estimating the RTT, must be 0 <= alpha <= 1",
                   DoubleValue (0.125),
                   MakeDoubleAccessor (&RttMeanDeviation::m_alpha),
                   MakeDoubleChecker<double> (0, 1))
    .AddAttribute ("Beta",
                   "Gain used in estimating the RTT variation, must be 0 <= beta <= 1",
                   DoubleValue (0.25),
                   MakeDoubleAccessor (&RttMeanDeviation::m_beta),
                   MakeDoubleChecker<double> (0, 1))
    
  ;
  return tid;
}

RttMeanDeviation::RttMeanDeviation()
{
  NS_LOG_FUNCTION (this);
}

RttMeanDeviation::RttMeanDeviation (const RttMeanDeviation& c)
  : RttEstimator (c), m_alpha (c.m_alpha), m_beta (c.m_beta)
{
  NS_LOG_FUNCTION (this);
}

TypeId
RttMeanDeviation::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
RttMeanDeviation::CheckForReciprocalPowerOfTwo (double val) const
{
  // NS_LOG_FUNCTION (this << val);
  if (val < TOLERANCE)
    {
      return 0;
    }
  // supports 1/32, 1/16, 1/8, 1/4, 1/2
  if (std::abs (1/val - 8) < TOLERANCE)
    {
      return 3;
    }
  if (std::abs (1/val - 4) < TOLERANCE)
    {
      return 2;
    }
  if (std::abs (1/val - 32) < TOLERANCE)
    {
      return 5;
    }
  if (std::abs (1/val - 16) < TOLERANCE)
    {
      return 4;
    }
  if (std::abs (1/val - 2) < TOLERANCE)
    {
      return 1;
    }
  return 0;
}

void
RttMeanDeviation::FloatingPointUpdate (Time m)
{
  // NS_LOG_FUNCTION (this << m);

  // EWMA formulas are implemented as suggested in
  // Jacobson/Karels paper appendix A.2

  // SRTT <- (1 - alpha) * SRTT + alpha *  R'
  Time err (m - m_estimatedRtt);
  double gErr = err.ToDouble (Time::S) * m_alpha;
  m_estimatedRtt += Time::FromDouble (gErr, Time::S);

  // RTTVAR <- (1 - beta) * RTTVAR + beta * |SRTT - R'|
  Time difference = Abs (err) - m_estimatedVariation;
  m_estimatedVariation += Time::FromDouble (difference.ToDouble (Time::S) * m_beta, Time::S);

  return;
}

void
RttMeanDeviation::IntegerUpdate (Time m, uint32_t rttShift, uint32_t variationShift)
{
  // NS_LOG_FUNCTION (this << m << rttShift << variationShift);
  // Jacobson/Karels paper appendix A.2
  int64_t meas = m.GetInteger ();
  int64_t delta = meas - m_estimatedRtt.GetInteger ();
  int64_t srtt = (m_estimatedRtt.GetInteger () << rttShift) + delta;
  m_estimatedRtt = Time::From (srtt >> rttShift);
  if (delta < 0)
    {
      delta = -delta;
    }
  delta -= m_estimatedVariation.GetInteger ();
  int64_t rttvar = m_estimatedVariation.GetInteger () << variationShift;
  rttvar += delta;
  m_estimatedVariation = Time::From (rttvar >> variationShift);
  return;
}

void 
RttMeanDeviation::Measurement (Time m)
{
  m_estimated_rtt_vector.push_back(m_estimatedRtt.GetMilliSeconds());
  m_actual_rtt_vector.push_back(m.GetMilliSeconds()); 
  MyRttFile1 <<m_estimatedRtt.GetMilliSeconds()<<" "<< m.GetMilliSeconds()<<std::endl;
  if (m_nSamples)
    { 
      // If both alpha and beta are reciprocal powers of two, updating can
      // be done with integer arithmetic according to Jacobson/Karels paper.
      // If not, since class Time only supports integer multiplication,
      // must convert Time to floating point and back again
      uint32_t rttShift = CheckForReciprocalPowerOfTwo (m_alpha);
      uint32_t variationShift = CheckForReciprocalPowerOfTwo (m_beta);
      if (rttShift && variationShift)
        {
          IntegerUpdate (m, rttShift, variationShift);
        }
      else
        {
          FloatingPointUpdate (m);
        }
    }
  else
    { // First sample
      m_estimatedRtt = m;               // Set estimate to current
      m_estimatedVariation = m / 2;  // And variation to current / 2
      NS_LOG_DEBUG ("(first sample) m_estimatedVariation += " << m);
    }
  m_nSamples++;
}

Ptr<RttEstimator> 
RttMeanDeviation::Copy () const
{
  NS_LOG_FUNCTION (this);
  return CopyObject<RttMeanDeviation> (this);
}

void 
RttMeanDeviation::Reset ()
{ 
  NS_LOG_FUNCTION (this);
  RttEstimator::Reset ();
}

RttMeanDeviation::~RttMeanDeviation ()
{
  NS_LOG_DEBUG("RttMeanDeviation destructor");
  // ==========================================AMAR ADD KORA ================================================>
  if (m_actual_rtt_vector.size() == 0)
  {
    MyRttFile2<<"RttMeanDeviation: Destructor m_actual_rtt_vector.size() = 0 found\n"; 
  }
}



// ==========================================AMAR ADD KORA ================================================>

// double CalcMHWScore(std::vector<int> scores)
// {
//   size_t size = scores.size();

//   if (size == 0)
//   {
//     return 0;  // Undefined, really.
//   }
//   else
//   {
//     sort(scores.begin(), scores.end());
//     if (size % 2 == 0)
//     {
//         double median1 = 1.0* (scores[size / 2 - 1] + scores[size / 2]); 
//         median1 = median1/2.0; 
//         return  median1;
//     }
//     else 
//     {
//         double median1 = 1.0* (scores[size / 2]); 
//         return  median1;
//     }
//   }
// }


NS_OBJECT_ENSURE_REGISTERED(RttMyWeightedMedian);

// Public

TypeId 
RttMyWeightedMedian::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RttMyWeightedMedian")
    .SetParent<RttEstimator> ()
    .SetGroupName ("Internet")
    .AddConstructor<RttMyWeightedMedian> ()
    .AddAttribute ("Alpha",
                   "Weight sharing parameter, must be 0 <= alpha <= 1",
                   DoubleValue ((1.0*7)/8),
                   MakeDoubleAccessor (&RttMyWeightedMedian::m_alpha),
                   MakeDoubleChecker<double> (0, 1))
    .AddAttribute ("Beta",
                   "Gain used in estimating the RTT variation, must be 0 <= beta <= 1",
                   DoubleValue ((1.0/2.0)),
                   MakeDoubleAccessor (&RttMyWeightedMedian::m_beta),
                   MakeDoubleChecker<double> (0, 1))
    // ==========================================AMAR ADD KORA ================================================>
    //RttMyWeightedMedian::GetTypeId  te AddAttribute 
    //https://www.nsnam.org/docs/manual/html/attributes.html   
    .AddAttribute ("m_observedkoyta_M",
                   "m_observedkoyta_M Means how many Prev Actual RTT value used to calculated new Weighted Medain RTT",
                   IntegerValue (5),
                   MakeIntegerAccessor (&RttMyWeightedMedian::m_observedkoyta_M),
                   MakeIntegerChecker<int32_t> ())
  ;
  return tid;
}

RttMyWeightedMedian::RttMyWeightedMedian()
{      
  MyRttFile5<<"RttMyWeightedMedian constructor; Median Func added\n"; 
  
  InitializeConstant();
}

RttMyWeightedMedian::RttMyWeightedMedian (const RttMyWeightedMedian& c)
  : RttEstimator (c), m_alpha (c.m_alpha), m_beta (c.m_beta)
{
  // Initialize member variables
  
  InitializeConstant(); 
}

TypeId
RttMyWeightedMedian::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void RttMyWeightedMedian::Measurement(Time measure)
{ 
    m_estimated_rtt_vector.push_back(m_estimatedRtt.GetMilliSeconds());
    m_actual_rtt_vector.push_back(measure.GetMilliSeconds()); 
    MyRttFile4 <<m_estimatedRtt.GetMilliSeconds()<<" "<< measure.GetMilliSeconds()<<std::endl;
    // *rttStream->GetStream () << Simulator::Now ().GetSeconds () << " " << newval.GetSeconds () << std::endl;
    NS_LOG_DEBUG("RttMyWeightedMedian In measurement");
    if (m_nSamples)
    {        

        NS_LOG_DEBUG("Actual rtt:" << measure.GetMilliSeconds());

        // 1) Get the predicted RTT from previous losses and assign to private variable

        // so far koyta estimate korsi?
        // eta ki >5 ? (>M)? if yes ; then M ta; otherwise oi size tai 

        int sizevectorbj = m_observedkoyta_M; 
        // MANE MEDIAN vector er size koto nibo?
        if((int) m_estimated_rtt_vector.size() < m_observedkoyta_M){
          sizevectorbj = m_estimated_rtt_vector.size(); 
        }




        // double alpha1 = (1.0*7)/(1.0*8); //focus this alpha1 is NOT THE SAME AS JACOBSON alpha 
        // header file eo dekhte pari ami chaile 

        std::vector<double> bj_vector;
        //ns3 gives error if I give vector a size
        //so I have to push_back; tai push_back korsi 
        for(int i=0; i<sizevectorbj; i++){
          double bj_val = pow(m_alpha, i-1); 
          bj_vector.push_back(bj_val); 

        }

        double constant_a1 = getConstantA1(); 
        double m_prev_rtt = m_estimatedRtt.ToDouble(Time::S);


        std::vector<double> rtt_estimate_vector_my; 
        //ai = Median(A1.ai−1, B1.mi−1, B2.mi−2, ..., BM.mi−M)    THIS IS MY EQUATION 

        // SO AMI VECTOR E : A1.ai−1, B1.mi−1, B2.mi−2, ..., BM.mi−M    egula push kore 
        // then median calculate korbo 
        int rtt_estimate_vector_size = sizevectorbj + 1;

        //vector size = M +1  = sizevectorbj  + 1 ( TO BE ACCURATE)
        
        rtt_estimate_vector_my.push_back(m_prev_rtt*constant_a1); 

        for(int i = 0 ; i< sizevectorbj ; i++){
          //vec[0] already push kore felsi 

          // focus: mi−1  = THIK AGER ACTUAL RTT
          // mi-2 = 2 GHOR ager actual RTT 

          double m_observed_rtt = m_actual_rtt_vector[m_actual_rtt_vector.size()- i]; 
          //foucs eta ; eqn e 

          double value_idx_i = m_observed_rtt*bj_vector[i]; 
          rtt_estimate_vector_my.push_back(value_idx_i); 

        }
        if((int) rtt_estimate_vector_my.size() != rtt_estimate_vector_size){
          MyRttFile5<<"size mismatched\n"; 
        }

        double myPredictedRtt = medianVectorCalcMy(rtt_estimate_vector_my); 
        //eta ami agei define kore rakhi  "medianVectorCalcMy"

        double prevMyEstimatedRtt = m_estimatedRtt.ToDouble(Time::S);
        m_estimatedRtt = Time::FromDouble (myPredictedRtt, Time::S); //m_estimatedRtt ===> SAVE hosse ===> 

        NS_LOG_DEBUG("Estimated rtt:" << m_estimatedRtt.GetMilliSeconds());

        

        // Variatio set kora 

        double prevMyRttVar = m_estimatedVariation.ToDouble(Time::S);
        double newMyRttVar = (1 - m_beta) * prevMyRttVar + m_beta * (abs(measure.ToDouble(Time::S) - prevMyEstimatedRtt));

        // Time err (measure - m_estimatedRtt);
        // double gErr = err.ToDouble (Time::S) * m_alpha;
        // m_estimatedRtt += Time::FromDouble (gErr, Time::S);
        // m_estimatedRtt += 0.01; 
        m_estimatedVariation = Time::FromDouble (newMyRttVar, Time::S);
      
      
    }
    else
    { // First sample
      m_estimatedRtt = measure;               // Set estimate to current
      m_estimatedVariation = measure / 2;  // And variation to current / 2
      NS_LOG_DEBUG ("(first sample) m_estimatedVariation += " << measure);
    }
    
}

Ptr<RttEstimator> 
RttMyWeightedMedian::Copy () const
{
  return CopyObject<RttMyWeightedMedian> (this);
}

void 
RttMyWeightedMedian::Reset ()
{ 
  // RttEstimator::Reset ();
}

RttMyWeightedMedian::~RttMyWeightedMedian ()
{
  if (m_actual_rtt_vector.size() == 0)
  {
    MyRttFile5<<"RttMyWeightedMedian: Destructor m_actual_rtt_vector.size() = 0 found\n"; 
  }
}

// Private

void RttMyWeightedMedian::InitializeConstant()
{ 

  m_alpha = (1.0*7)/(1.0*8);
  m_beta = 0.5;
 
  m_observedkoyta_M = 5; 
   

}



} //namespace ns3
