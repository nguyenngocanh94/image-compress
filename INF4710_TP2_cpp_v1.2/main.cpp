
// INF4710 H2016 TP2 v1.2

#include "TP2Utils.h"
typedef std::pair<std::string, int> ImagePathFlag;

//template<typename T = uchar>

int main(int /*argc*/, char** /*argv*/) {
    try {
        // ...
		const std::vector<ImagePathFlag> vsTestImages = {
			{ "INF4710_TP2_Fichiers/car.bmp", cv::IMREAD_COLOR },
			{ "INF4710_TP2_Fichiers/lena.png", cv::IMREAD_COLOR },
			{ "INF4710_TP2_Fichiers/logo.tif", cv::IMREAD_COLOR },
			{ "INF4710_TP2_Fichiers/logo_noise.tif", cv::IMREAD_COLOR },
			{ "INF4710_TP2_Fichiers/lion.bmp", cv::IMREAD_GRAYSCALE },
			{ "INF4710_TP2_Fichiers/fleurs.bmp", cv::IMREAD_GRAYSCALE },

			
		};

		for (const ImagePathFlag& oImagePathFlag : vsTestImages) {

			cv::Mat oInputImg = cv::imread(oImagePathFlag.first, oImagePathFlag.second);
			if (oInputImg.empty())
				CV_Error(-1, "Could not load image!");
			
			std::vector<cv::Mat_<uchar>> vectorBlock = decoupage(oInputImg); // Decoupage en bloc 8x8 means : split Mat image to Vector(block(8*8).imageChannel)
			std::vector<cv::Mat_<uchar>> ivectorBlock;
			
			int outputsize1 = 0, outputsize2 = 0, outputsize3 = 0;
			
			for (int l = 0; l < vectorBlock.size(); ++l){

				cv::Mat_<float> dctMat = dct(vectorBlock[l]); // Application de la DCT sur un bloc 8x8 means DCT each block 's saved in vector

				cv::Mat_<_int16> Matqant10(8, 8, CV_16SC1), Matqant50(8, 8, CV_16SC1), Matqant90(8, 8, CV_16SC1);
				std::vector<_int16> zigzag1, zigzag2, zigzag3;

				cv::divide(dctMat, quantif(10), Matqant10, CV_16SC1); // Quantification de la DCT avec Q = 10
				cv::divide(dctMat, quantif(50), Matqant50, CV_16SC1); // Quantification de la DCT avec Q = 50
				cv::divide(dctMat, quantif(90), Matqant90, CV_16SC1); // Quantification de la DCT avec Q = 90

				zigzag1 = zigzag(Matqant10); // Application du ziggzag means zigzag scan for block 8*8
				outputsize1 +=mat2huff(zigzag1).code.size(); // Application de l algo de Huffman puis recuperation de la taille du code en sortie

				zigzag2 = zigzag(Matqant50); // Application du ziggzag
				outputsize2 += mat2huff(zigzag2).code.size();  // Application de l algo de Huffman puis recuperation de la taille du code en sortie

				zigzag3 = zigzag(Matqant90); // Application du ziggzag
				outputsize3 += mat2huff(zigzag3).code.size();  // Application de l algo de Huffman puis recuperation de la taille du code en sortie

				cv::Mat_<uchar> idctMat = idct(dctMat); // Application de la DCT inverse sur un bloc 8x8

				ivectorBlock.push_back(idctMat); // Ajout des blocs dans le vecteur

				}

				
				std::cout << " Taux de compression : " << (float)(8 * 64 * vectorBlock.size()) / outputsize1 << " " << (float)(8 * 64 * vectorBlock.size()) / outputsize2 << " " << (float)(8 * 64 * vectorBlock.size()) / outputsize3 << std::endl;

				cv::Mat Output = idecoupage(ivectorBlock, oInputImg.size(), oInputImg.channels()); // Reconstitution de l image
				cv::Mat Matdiff;
				cv::absdiff(oInputImg, Output, Matdiff); // Difference des images

				cv::imshow("Image normal", oInputImg);
				cv::imshow("Image reconstitue", Output);
				cv::imshow("Difference", Matdiff);
				cv::waitKey(0);



			} 
			// Test Bench pour le codage MPEGs
			test_match(1, 1);
			test_match(5, 400);
			test_match(10, 999);
			test_match(18, 726);

    }
    catch(const cv::Exception& e) {
        std::cerr << "Caught cv::Exceptions: " << e.what() << std::endl;
    }
    catch(const std::runtime_error& e) {
        std::cerr << "Caught std::runtime_error: " << e.what() << std::endl;
    }
    catch(...) {
        std::cerr << "Caught unhandled exception." << std::endl;
    }
    return 0;
}
