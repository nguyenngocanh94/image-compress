
#pragma once
#include "TP2Utils.h"

// decoupage: reformats a 2D or 3D image (i.e. may be multi-channel) to a uint8 block array (where blocks are always 8x8)
template<typename T=uchar>
inline std::vector<cv::Mat_<T>> decoupage(const cv::Mat& oImage) {
	CV_Assert(oImage.depth()==CV_8U);

	int nbBlocbycols = oImage.cols/ 8; // nombre de bloc par colonne
	int nbBlocbyrows = oImage.rows / 8;// nombre de bloc par ligne
	std::vector<cv::Mat_<T>> blocMat(nbBlocbycols * nbBlocbyrows *oImage.channels());
	std::vector<cv::Mat_<T>> rgbChannels(oImage.channels());
	
	if (oImage.channels() == 1){
		rgbChannels[0] = oImage;
	}
	else{
		cv::split(oImage, rgbChannels);
	}

	for (int c = 0; c < rgbChannels.size(); ++c){
		for (int i = 0; i<nbBlocbyrows; ++i) {
			for (int j = 0; j<nbBlocbycols; ++j) {
				blocMat[c*nbBlocbyrows *nbBlocbycols + i * nbBlocbycols + j] = rgbChannels[c].cv::Mat::colRange(j * 8, (j + 1) * 8).cv::Mat::rowRange(i * 8, (i + 1) * 8);
			}
		}
	}

	return blocMat;
}