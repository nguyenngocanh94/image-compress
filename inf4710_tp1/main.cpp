
// INF4710 H2016 TP1 v1.2

#include <opencv2/opencv.hpp>
#include <exception>
#include <iostream>

typedef std::pair<uchar, uchar> RLECode; // first = symbol, second = count
typedef std::pair<std::string, int> ImagePathFlag; // first = path, second = imread flag

struct FreemanChain {
	enum FreemanCode {
		eFreemanCode_North = 0, // (same order as INF4710 course notes)
		eFreemanCode_NorthEast,
		eFreemanCode_East,
		eFreemanCode_SouthEast,
		eFreemanCode_South,
		eFreemanCode_SouthWest,
		eFreemanCode_West,
		eFreemanCode_NorthWest,
	};
	std::vector<FreemanCode> vChain; // contains the chain composed of linked shape boundary pixels
	cv::Point2i oInitPos; // initial point used to start the encoding process
	int nChannelId; // channel index where the shape was found, and where it should be reconstructed
	size_t size() { // return the encoded (byte) size of this freeman-coded chain
		return vChain.size() + sizeof(oInitPos) + sizeof(nChannelId);
	}
};

// detectBinaryEdges: used as a cheap way to detect shape edges in binary images (*OPTIONAL FOR FREEMAN ENCODING/DECODING*)
cv::Mat detectBinaryEdges(const cv::Mat& oInputBinaryImage) {
	CV_Assert(!oInputBinaryImage.empty());
	cv::Mat oOutputEdgeImage;
	cv::dilate(oInputBinaryImage, oOutputEdgeImage, cv::Mat());
	oOutputEdgeImage = oOutputEdgeImage & ~oInputBinaryImage;
	return oOutputEdgeImage;
}

// deformatInput: reconstructs a 2D/3D image (depending on channel count) from a 1D byte vector.
cv::Mat deformatInput(const std::vector<uchar>& vSignal, const cv::Size& oImageSize, int nChannels) {
	CV_Assert(!vSignal.empty());
	CV_Assert(vSignal.size() == (size_t)oImageSize.area()*nChannels);
	// note: in opencv, rows = height, and cols = width
	cv::Mat oReconstructedImage(oImageSize, CV_8UC(nChannels));
	// note: in OpenCV, data is stored in row-major format, and channels are interlaced!
	if (nChannels == 1)
		std::copy(vSignal.begin(), vSignal.end(), oReconstructedImage.data);
	else {
		for (int c = 0; c<oReconstructedImage.channels(); ++c) {
			for (int i = 0; i<oReconstructedImage.rows; ++i) {
				for (int j = 0; j<oReconstructedImage.cols; ++j) {
					const size_t nSignalIdx = c*(oReconstructedImage.rows*oReconstructedImage.cols) + i*(oReconstructedImage.cols) + j;
					const size_t nImageIdx = i*(oReconstructedImage.cols*oReconstructedImage.channels()) + j*(oReconstructedImage.channels()) + c;
					oReconstructedImage.data[nImageIdx] = vSignal[nSignalIdx];
				}
			}
		}
	}
	return oReconstructedImage;
}

// formatInput: reformats a 2D or 3D image (i.e. may be multi-channel) to a 1D byte vector.
std::vector<uchar> formatInput(const cv::Mat& oInputImg) {
	CV_Assert(!oInputImg.empty());
	std::vector<uchar> vSignal(oInputImg.cols*oInputImg.rows*oInputImg.channels());

	// ... @@@@ TODO (hint: OpenCV's cv::Mat uses an array of uchar as its internal data type!)
	if (oInputImg.channels() == 1)
		std::copy(oInputImg.datastart, oInputImg.dataend, vSignal.begin());
	else {
		for (int i = 0; i<oInputImg.rows; ++i) {
			for (int j = 0; j<oInputImg.cols; ++j) {
				for (int c = 0; c<oInputImg.channels(); ++c) {
					const size_t nImageIdx = i*(oInputImg.cols*oInputImg.channels()) + j*(oInputImg.channels()) + c; // Indice du pixel a recuperer dans l image d origine 
					const size_t nSignalIdx = c*(oInputImg.rows*oInputImg.cols) + i*(oInputImg.cols) + j; // Indice du pixel a placer dans l image de sortie
					vSignal[nSignalIdx] = oInputImg.data[nImageIdx];
				}
			}
		}
	}
	return vSignal; // decent compilers will use return-value-optimization or move constructor to avoid copies
}

// decodeRLE: reads an encoded RLE signal (1D byte vector), and decodes it into its original signal.
std::vector<uchar> decodeRLE(const std::vector<RLECode>& vInputSignal) {
	CV_Assert(!vInputSignal.empty());
	std::vector<uchar> vOutputSignal;
	vOutputSignal.reserve(640 * 480 * 3); // worse case scenario, 640x480 image with 3 channels
	for (size_t i = 0; i<vInputSignal.size(); ++i)
		for (size_t j = 0; j<vInputSignal[i].second; ++j)
			vOutputSignal.push_back(vInputSignal[i].first);
	return vOutputSignal; // decent compilers will use return-value-optimization or move constructor to avoid copies
}

// encodeRLE: encodes a signal (1D byte vector) using a run-length-encoding strategy.
std::vector<RLECode> encodeRLE(const std::vector<uchar>& vInputSignal) {
	CV_Assert(!vInputSignal.empty());
	std::vector<RLECode> vOutputSignal;

	// ... @@@@ TODO
	RLECode pair; // Declaration d une paire RLE
	pair.first = vInputSignal[0]; // Initialisation du symbole avec le premier element 
	pair.second = 1; // Initialisation du nombre d occurences du premier element a 1
	
	
		for (size_t i = 1; i < vInputSignal.size(); ++i){

			if (vInputSignal[i] == pair.first && pair.second < 255){ // Incrementation du nombre d occurence si obtention du meme symbole
			pair.second++;
		}
		else{ // Si les  symboles consecutifs sont different  alors on ajoute la pair et on creer une nouvelle pair en initalisant les valeurs
			vOutputSignal.push_back(pair);
			pair.first = vInputSignal[i];
			pair.second = 1;
		}

	}
	vOutputSignal.push_back(pair); // On ajoute la derniere pair
	int t = 0;
	for (size_t i = 0; i < vOutputSignal.size(); ++i){

		t += vOutputSignal[i].second;
	}
	return vOutputSignal; // decent compilers will use return-value-optimization or move constructor to avoid copies
}

// decodeFreeman: decodes a Freeman signal and reconstructs its original 2D/grayscale or 3D/multi-channel matrix.
// NOTE: if you do not use a dilation-based method to detect shape edges (i.e. detectBinaryEdges), you should set bErode to false
cv::Mat decodeFreeman(const std::vector<FreemanChain>& vSignal, const cv::Size& oImageSize, int nChannels, bool bErode = true) {
	cv::Mat oReconstructedImage(oImageSize, CV_8UC(nChannels), cv::Scalar_<uchar>::all(0));
	for (size_t i = 0; i<vSignal.size(); ++i) {
		std::vector<cv::Point2i> vShapeContourPts(vSignal[i].vChain.size() + 1);
		vShapeContourPts[0] = vSignal[i].oInitPos;
		for (size_t j = 0; j<vSignal[i].vChain.size(); ++j) {
			switch (vSignal[i].vChain[j]) {
			case FreemanChain::eFreemanCode_North:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(0, -1); break;
			case FreemanChain::eFreemanCode_NorthEast:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(1, -1); break;
			case FreemanChain::eFreemanCode_East:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(1, 0); break;
			case FreemanChain::eFreemanCode_SouthEast:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(1, 1); break;
			case FreemanChain::eFreemanCode_South:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(0, 1); break;
			case FreemanChain::eFreemanCode_SouthWest:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(-1, 1); break;
			case FreemanChain::eFreemanCode_West:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(-1, 0); break;
			case FreemanChain::eFreemanCode_NorthWest:
				vShapeContourPts[j + 1] = vShapeContourPts[j] + cv::Point2i(-1, -1); break;
			}
		}
		cv::Mat oReconstructedPoly(oReconstructedImage.size(), oReconstructedImage.type(), cv::Scalar_<uchar>::all(0));
		cv::Scalar_<uchar> vColor = cv::Scalar_<uchar>::all(0);
		vColor[vSignal[i].nChannelId] = 255;
		cv::fillPoly(oReconstructedPoly, std::vector<std::vector<cv::Point2i>>(1, vShapeContourPts), vColor);
		oReconstructedImage |= oReconstructedPoly;
	}
	if (bErode) // erode is only necessary if the initial edge image was obtained via dilation (i.e. using 'detectBinaryEdges')
		cv::erode(oReconstructedImage, oReconstructedImage, cv::Mat());
	return oReconstructedImage;
}

// encodeFreeman: encodes a 2D/grayscale or 3D/multi-channel matrix using a Freeman encoding strategy.
std::vector<FreemanChain> encodeFreeman(const cv::Mat& oInputImg) {
	CV_Assert(!oInputImg.empty());
	std::vector<FreemanChain> vOutputSignal;
	cv::Mat edgesMatrix;


	// ... @@@@ TODO (hint: use 'detectBinaryEdges'!)
	std::vector<cv::Point2i> vDirectionPoint = { // vecteur de points de direction (Nord, Ouest, Est , Sud)
		cv::Point2i(0, -1),
		cv::Point2i(1, 0),
		cv::Point2i(0, 1),
		cv::Point2i(-1, 0),
		cv::Point2i(1, -1),
		cv::Point2i(1, 1),
		cv::Point2i(-1, 1),
		cv::Point2i(-1, -1),
	};


	std::vector<FreemanChain::FreemanCode> vDirectionCode = { // vecteur de points de code de freeman (Nord=0, Ouest, Est , Sud) 
		FreemanChain::eFreemanCode_North,
		FreemanChain::eFreemanCode_East,
		FreemanChain::eFreemanCode_South,
		FreemanChain::eFreemanCode_West,
		FreemanChain::eFreemanCode_NorthEast,
		FreemanChain::eFreemanCode_SouthEast,
		FreemanChain::eFreemanCode_SouthWest,
		FreemanChain::eFreemanCode_NorthWest,
	};

	edgesMatrix = detectBinaryEdges(oInputImg); // Detection des bords/contours par detectBinaryEdges
	for (int i = 0; i<edgesMatrix.rows; ++i) {
		for (int j = 0; j<edgesMatrix.cols; ++j) {
			for (int c = 0; c<edgesMatrix.channels(); ++c) { // On parcours l image jusqu a que l on trouve un point de contours

				FreemanChain chain; // Creation d une chaine de freeman
				const int channels = edgesMatrix.channels();
				const size_t nbElementbyRow = edgesMatrix.cols*channels; // Nombre d elements par lignes
				const size_t nImageIdx = i*(nbElementbyRow)+j*(channels)+c; // Indice de parcours dans notre matrice

				if (edgesMatrix.data[nImageIdx]){ // Des que l on trouve un point de contours ( !=0 )
					int I = i; int J = j; 
					size_t pointTemp = nImageIdx;

					chain.oInitPos = cv::Point2i(J, I); // Point d initialisation
					chain.nChannelId = c; // Couleur c

					while (true) { // Tant qu il parcourt le contours
						bool edgeDetect = true;
						for (int t = 0; t < vDirectionPoint.size(); ++t){
							int x = vDirectionPoint[t].x;
							int y = vDirectionPoint[t].y;
							size_t output = y * nbElementbyRow + x * channels; // 
							if (I > 0 && I <edgesMatrix.rows && J > 0 && J <edgesMatrix.cols){ // Conditions pour les effets de bords
								if (edgesMatrix.data[pointTemp + output]){ // Si le point a proximite est un point de contours
									chain.vChain.push_back(vDirectionCode[t]); // Ajout dans la chaine
									edgesMatrix.data[pointTemp] = 0; // Suppression du point actuel
									I += y; J += x; // Le point actuel devient le point detecte qui est le point a proximite
									pointTemp = pointTemp + output;
									edgeDetect = false; // Flag pour detection
									break;

								}
							}

						}
						if (edgeDetect){ // Si on arrive au dernier point de contours alors on le supprime et on sort de la boucle while
							edgesMatrix.data[pointTemp] = 0;
							break;
						}

					}
					vOutputSignal.push_back(chain); // Ajout de la chaine


				}
			}
		}

	}
	return vOutputSignal; // cv::Mat's are like shared pointers, no problem here!
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char** /*argv*/) {
	try {
		// note: by default, on most platforms, imread always returns 3-ch images unless the cv::IMREAD_GRAYSCALE flag is set (here we hardcode it based on prior knowledge)
		const std::vector<ImagePathFlag> vsTestImages = {
			{ "TP1_H2016_images/test0.png", cv::IMREAD_GRAYSCALE },
			{ "TP1_H2016_images/test2.png", cv::IMREAD_GRAYSCALE },
			{ "TP1_H2016_images/test3.png", cv::IMREAD_GRAYSCALE },
			{ "TP1_H2016_images/test4.png", cv::IMREAD_COLOR },
			{ "TP1_H2016_images/test5.png", cv::IMREAD_COLOR },
			{ "TP1_H2016_images/test6.png", cv::IMREAD_COLOR },
			{ "TP1_H2016_images/test7.png", cv::IMREAD_COLOR },
		};
		cv::Mat vSignalRLEDef, vFreemanDec, vSignalRLEDiff, vFreemanDecDiff;
		std::vector<uchar> vSignal, vSignalRLEDec;
		std::vector<RLECode> vSignalRLE;
		std::vector<FreemanChain> vFreeman;
		for (const ImagePathFlag& oImagePathFlag : vsTestImages) {
			cv::Mat oInputImg = cv::imread(oImagePathFlag.first, oImagePathFlag.second);
			if (oInputImg.empty())
				CV_Error(-1, "Could not load image!");

			// ... @@@@ TODO (Make sure decoding also provides the original image!)

			vSignal = formatInput(oInputImg); // RGBRGB => RRGGBB
			vSignalRLE = encodeRLE(vSignal);  // Encodage de la chaine RRGGBB avec le codage RLE 
			vSignalRLEDec = decodeRLE(vSignalRLE);// Decodage de la chaine RLE
			vSignalRLEDef = deformatInput(vSignalRLEDec, oInputImg.size(), oInputImg.channels()); //   RRGGBB =>  RGBRGB

			vFreeman = encodeFreeman(oInputImg); // Encodage de l image avec le codage en chaine de Freeman 
			vFreemanDec = decodeFreeman(vFreeman, oInputImg.size(), oInputImg.channels(), true); // Decodage de la chaine de Freeman

			//Affichage des images recodees
			cv::imshow("Image normal", oInputImg);
			cv::imshow("Image Reencode RLE", vSignalRLEDef);
			cv::imshow("Image Reencode Freeman", vFreemanDec);
			cv::waitKey(0);
			
			//Calcul des tailles des images en sorties
			size_t sizeFreeman = 0;
			for (int i = 0; i < vFreeman.size(); ++i){
				sizeFreeman += vFreeman[i].size();
			}

			int tCompRLE,tCompFreeman; // Taux de Compression
			tCompRLE = oInputImg.size().area() * oInputImg.channels() / (vSignalRLE.size() * 2);
			tCompFreeman = oInputImg.size().area() * oInputImg.channels() / sizeFreeman;

			std::cout << " Taux de Compression RLE : " << tCompRLE << std::endl;
			std::cout << " Taux de Compression Freeman : " << tCompFreeman << std::endl;

			// Affichage des differences entre l image en entree et l image en sortie
			cv::absdiff(oInputImg, vSignalRLEDef, vSignalRLEDiff);
			cv::absdiff(oInputImg, vFreemanDec, vFreemanDecDiff);
			cv::imshow("Difference Image Normal avec RLE", vSignalRLEDiff);
			cv::imshow("Difference Image Normal avec Freeman", vFreemanDecDiff);
			cv::waitKey(0);
		}
	}
	catch (const cv::Exception& e) {
		std::cerr << "Caught cv::Exceptions: " << e.what() << std::endl;
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Caught std::runtime_error: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Caught unhandled exception." << std::endl;
	}
	return 0;
}
