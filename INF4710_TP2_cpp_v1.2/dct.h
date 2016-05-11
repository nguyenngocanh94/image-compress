
#pragma once
#include "TP2Utils.h"

// dct: computes the discrete cosinus tranform of a 8x8 block
template<typename Tin=uchar,typename Tout=float>
inline cv::Mat_<Tout> dct(const cv::Mat_<Tin>& oBlock) {
	int k = 0;
	int N = 8; 
	cv::Mat M(N, N, CV_32FC1);
	cv::Mat_<Tout> P(N, N, CV_32FC1), Dct(N, N, CV_32FC1);

	oBlock.convertTo(M, CV_32FC1); // Conversion de uchar en float

	//Remplissage de la matrice P de cosinus
	for (int i = 1; i < P.rows; i++){
		for (int j = 0; j < P.cols; j++){
			P.at<float>(i, j, k) = sqrt(2) / sqrt(N) * cos((2 * j + 1)* i * CV_PI / (2 * N));
		}
	}

	//Remplissage de la premiere ligne pour i=0
	for (int j = 0; j < P.cols; j++){

		P.at<float>(0, j, k) = 1.0 / sqrt(N);

	}
	
	// Dct = P * M * P.t()

	Dct = (P * M)  ;
	Dct = Dct * P.t();

	return Dct;

}
