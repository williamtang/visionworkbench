// __BEGIN_LICENSE__
//
// Copyright (C) 2006 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration
// (NASA).  All Rights Reserved.
//
// Copyright 2006 Carnegie Mellon University. All rights reserved.
//
// This software is distributed under the NASA Open Source Agreement
// (NOSA), version 1.3.  The NOSA has been approved by the Open Source
// Initiative.  See the file COPYING at the top of the distribution
// directory tree for the complete NOSA document.
//
// THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY
// KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT
// LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO
// SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
// A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT
// THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT
// DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE.
//
// __END_LICENSE__

#include <cxxtest/TestSuite.h>
#include <vw/InterestPoint/IntegralImage.h>
#include <vw/Image.h>
#include <vw/FileIO.h>

using namespace vw;
using namespace vw::ip;

class TestIntegral : public CxxTest::TestSuite
{
 public:

  void test_integral_summing() {
    ImageView<float> graffiti;
    read_image( graffiti, "sub.png" );
    ImageView<double> integral;
    integral = IntegralImage( graffiti );
    
    for (unsigned size = 10; size <= 70; size+= 10) {
      float sum = IntegralBlock( integral,
				 Vector2i(0,0),
				 Vector2i(size,size) );
      float actual_sum = 0;
      for (unsigned i = 0; i < size; i++ ) {
	for (unsigned j = 0; j < size; j++ ) {
	  actual_sum+=graffiti(i,j);
	}
      }

      TS_ASSERT_DELTA(actual_sum, sum, .01);
    }
  }

  void test_haar_filters() { 
    // Loading Image
    ImageView<float> graffiti, gradient;
    read_image( graffiti, "sub.png" );
    read_image( gradient, "noisy_gradient_60.png" );

    // Building Integrals
    ImageView<double> integral = IntegralImage( graffiti );
    
    float hand_response = 0;
    for ( unsigned i = 0; i < 2; i++ ) {
      for ( unsigned j = 0; j < 2; j++ ) {
	if ( i == 0 ) {
	  hand_response -= graffiti(i,j);
	} else {
	  hand_response += graffiti(i,j);
	}
      }
    }
    TS_ASSERT_DELTA( hand_response,
		     HHaarWavelet( integral,
				   1,1,2 ),
		     .001 );
    
    hand_response = 0;
    for ( unsigned i = 0; i < 2; i++ ) {
      for ( unsigned j = 0; j < 2; j++ ) {
	if ( j == 0 ) {
	  hand_response -= graffiti(i,j);
	} else {
	  hand_response += graffiti(i,j);
	}
      }
    }
    TS_ASSERT_DELTA( hand_response,
		     VHaarWavelet( integral,
				   1, 1, 2 ),
		     .001 );
  
  }

  void test_haar_filters2() {
    // Load Image
    ImageView<float> graffiti, gradient;
    read_image( graffiti, "sub.png" );
    read_image( gradient, "noisy_gradient_60.png" );
    
    // Rotating Image
    ImageView<float> graffiti_r, gradient_r;
    graffiti_r = rotate_180(graffiti);
    gradient_r = rotate_180(gradient);

    // Building Integrals
    ImageView<double> int_graf, int_graf_r, int_grad, int_grad_r;
    int_graf = IntegralImage( graffiti );
    int_graf_r = IntegralImage( graffiti_r);
    int_grad = IntegralImage( gradient );
    int_grad_r = IntegralImage( gradient_r );
    
    std::cout << int_grad_r(10,10) << " "
	      << int_grad_r(10.4,10) << " "
	      << int_grad_r(11,10) << std::endl;

    for ( unsigned i = 10; i < 90; i+=5 ) {
      for ( unsigned j = 10; j < 50; j+=5 ) {
	// Size 10
	TS_ASSERT_DELTA( -HHaarWavelet( int_graf, i,j, 10),
			 HHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 10 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_graf, i,j, 10),
			 VHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 10 ),
			 .001 );
	TS_ASSERT_DELTA( -HHaarWavelet( int_grad, i,j, 10),
			 HHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 10 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_grad, i,j, 10),
			 VHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 10 ),
			 .001 );

	// Size 20
	TS_ASSERT_DELTA( -HHaarWavelet( int_graf, i,j, 20),
			 HHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 20 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_graf, i,j, 20),
			 VHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 20 ),
			 .001 );
	TS_ASSERT_DELTA( -HHaarWavelet( int_grad, i,j, 20),
			 HHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 20 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_grad, i,j, 20),
			 VHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 20 ),
			 .001 );

	// Size 4
	TS_ASSERT_DELTA( -HHaarWavelet( int_graf, i,j, 4),
			 HHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 4 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_graf, i,j, 4),
			 VHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 4 ),
			 .001 );
	TS_ASSERT_DELTA( -HHaarWavelet( int_grad, i,j, 4),
			 HHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 4 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_grad, i,j, 4),
			 VHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 4 ),
			 .001 );

	// Size 16
	TS_ASSERT_DELTA( -HHaarWavelet( int_graf, i,j, 16),
			 HHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 16 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_graf, i,j, 16),
			 VHaarWavelet( int_graf_r, int_graf.cols()-1-i,
				       int_graf.rows()-1-j, 16 ),
			 .001 );
	TS_ASSERT_DELTA( -HHaarWavelet( int_grad, i,j, 16),
			 HHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 16 ),
			 .001 );
	TS_ASSERT_DELTA( -VHaarWavelet( int_grad, i,j, 16),
			 VHaarWavelet( int_grad_r, int_grad.cols()-1-i,
				       int_grad.rows()-1-j, 16 ),
			 .001 );
      }
    }
  }

  void test_derivative_filters () {
    ImageView<float> graffiti;
    read_image( graffiti, "sub.png" );
    ImageView<double> integral;
    integral = IntegralImage( graffiti );
    ImageView<float> rotated;
    rotated = rotate_180(graffiti);
    ImageView<double> r_integral;
    r_integral = IntegralImage( rotated );

    TS_ASSERT_DELTA( XSecondDerivative( integral,
					49, 49, 51 ), 
		     XSecondDerivative( r_integral,
					50, 50, 51 ),
		     .0001 );

    TS_ASSERT_DELTA( YSecondDerivative( integral,
					49, 49, 51 ),
		     YSecondDerivative( r_integral,
					50, 50, 51 ),
		     .0001 );

    TS_ASSERT_DELTA( XYDerivative( integral,
				   49, 49, 51 ),
		     XYDerivative( r_integral,
				   50, 50, 51 ),
		     .0001 );
   
  }
};
