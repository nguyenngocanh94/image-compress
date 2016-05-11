
#pragma once
#include "TP2Utils.h"

// izigzag: returns an 8x8 block created by dezig-zagging through a 64-element array
template<int nBlockSize=8,typename T=uchar>
inline cv::Mat_<T> izigzag(const std::vector<T>& vec) {
    CV_Assert(!vec.empty());
    CV_Assert(int(vec.size())==nBlockSize*nBlockSize);
    int nIdx = 0;
    cv::Mat_<T> oMatResult(nBlockSize*nBlockSize,1);
    for(int i=0; i<nBlockSize*2; ++i)
        for(int j=(i<nBlockSize)    ?    0   :  i-nBlockSize+1; j<=i && j<nBlockSize; ++j)
            oMatResult((i&1) ? j*(nBlockSize-1)+i         :(i-j)*nBlockSize+j) = vec[nIdx++];
    return oMatResult.reshape(0,nBlockSize);
}