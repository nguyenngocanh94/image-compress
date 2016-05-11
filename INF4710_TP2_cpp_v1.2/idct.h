
#pragma once
#include "TP2Utils.h"

// idct: computes the inverse discrete cosinus tranform of a 8x8 block
template<typename Tin=float,typename Tout=uchar>
inline cv::Mat_<Tout> idct(const cv::Mat_<Tin>& oBlock_c) {
	int k = 0;
	int N = 8; 
	cv::Mat M(N, N, CV_32FC1);
	cv::Mat_<Tin> P(N, N, CV_32FC1);
    cv::Mat_<Tout> IDct;
	
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


	// M = P * Dct * P.t()
	M = (P.t() * oBlock_c);
	M = M * P;

	M.convertTo(IDct, CV_8UC1); // Conversion de float en uchar

	return IDct;


}
