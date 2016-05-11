
#pragma once
#include "TP2Utils.h"

// idecoupage: reconstructs a 2D/3D image (depending on channel count) from a uint8 block array (where blocks are always 8x8).
template<typename T=uchar>
inline cv::Mat idecoupage(const std::vector<cv::Mat_<T>>& vBlocks, const cv::Size& oImageSize, int nChannels) {
	std::vector<cv::Mat_<T>> mergedMatrix(nChannels);
	cv::Mat outputImage;
	int nbBlocbycols = oImageSize.width / 8; // nombre de bloc par colonne
	int nbBlocbyrows = oImageSize.height / 8; // nombre de bloc par ligne

	for (int c = 0; c<nChannels; ++c) {
		cv::Mat t1;
		std::vector<cv::Mat> mt1;
		for (int i = 0; i<nbBlocbyrows; ++i) {
			cv::Mat t2;
			std::vector<cv::Mat> mt2;
			for (int j = 0; j<nbBlocbycols; ++j) {
				mt2.push_back(vBlocks[c*nbBlocbycols *nbBlocbyrows + i*nbBlocbycols + j]);		// Ajout du bloc dans le vecteur	
			}
			cv::hconcat(mt2,t2); // Concatenation de la matrice avec le vecteur (Ajout par colonne)
			mt1.push_back(t2); // Ajout du vecteur dans la matrice
			
		}
		cv::vconcat(mt1, t1); // Concatenation de la matrice avec le vecteur (Ajout par ligne)
		mergedMatrix[c].push_back(t1); // Ajout du vecteur dans la matrice finale
	}
	cv::merge(mergedMatrix, outputImage); // Transforme en matrice RGB
	return outputImage;
}
