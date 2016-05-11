
#pragma once
#include "TP2Utils.h"

// zigzag: returns a 64-element array created by zig-zagging through a 8x8 block
template<int nBlockSize = 8, typename T = int16_t>
inline std::vector<T> zigzag(const cv::Mat_<T>& mat) {
    CV_Assert(!mat.empty());
    CV_Assert(mat.rows==mat.cols && mat.rows==nBlockSize);
	int nIdx = 0;
	std::vector<T> zigzag(nBlockSize*nBlockSize);
	
	for (int i = 0; i < nBlockSize * 2; ++i)
		for (int j = (i < nBlockSize) ? 0 : i - nBlockSize + 1; j <= i && j < nBlockSize; ++j)
			zigzag[nIdx++] = mat((i & 1) ? j*(nBlockSize - 1) + i : (i - j)*nBlockSize + j);

	return zigzag;
}