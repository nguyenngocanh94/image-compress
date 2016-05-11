
#pragma once
#include "TP2Utils.h"

// match_block: finds the best match for a 8x8 block in a 16x16 region by minimizing mean squared error (EQM)
template<typename Tin=uchar>
inline cv::Point2i match_block(const cv::Mat_<Tin>& oBlock, const cv::Point2i& oBlockPos, const cv::Mat& oSearchImage, double& dEQM_min) {
    // return value : 2d displacement vector between oBlockPos and the newly found 8x8 region's top-left corner
    // oBlock : 8x8 image block taken from the source image
    // oBlockPos : 2d position vector of the top-left corner of 'oBlock'
    // oSearchImage : full image to be searched for local correspondances around block_pos
    // dEQM_min : minimal mean squared error found (output only)
    CV_Assert(oSearchImage.depth()==CV_8U);
	int i = oBlockPos.y;  int j = oBlockPos.x;  // Recuperation des coordonnes du bloc
	// Contraintes sur les frontieres de la zone 16x16 sur l image 
	int leftborder, rightborder, upborder, downborder;
	leftborder = MAX(j-4,0);
	rightborder = MIN(j + 11, oSearchImage.size().width -7 );
	upborder = MAX(i - 4, 0);
	downborder = MIN(i + 11, oSearchImage.size().height -7);

	double eqm_min = dEQM_min;
	cv::Point2i min_pos;
	for (int x = leftborder; x < rightborder; x++) { // Parcours des 81 valeurs possibles ( en prenant compte les frontieres)

		for (int y = upborder; y < downborder; y++) {

			cv::Mat region = oSearchImage.rowRange(y, y + 8).colRange(x, x + 8); // Recuperation du bloc 8x8
			
			double eqm = 0.0;

			for (int k = 0; k < 8; k++){
				for (int l = 0; l < 8; l++){
					eqm += pow(( oBlock.at<uchar>(k, l, 0) -  region.at<uchar>(k, l, 0)),2); // Calcul de l erreur quadratique EQM
				}
			}
			eqm = eqm / 64.0;

			if (eqm < eqm_min){ // Mis a jour de l EQM et des coordonnees du bloc le plus semblable
				eqm_min = eqm;
				min_pos = cv::Point2i(y, x);
			}
		}
	}
	dEQM_min = eqm_min;
	return min_pos;
}
