#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <iostream>
using namespace std;

int display = 1;
int checkExposure( cv::Mat &image) {
	const int peakMinCutoff = 5000;
	const int peakMaxCutoff = 50000;
	const int minWhite = 8;
	const int maxWhite = 14;
	int width = image.cols;
	int height = image.rows;
    int histSize = 16;
    int pixels = width*height;

	cv::Mat hist;

	cv::calcHist(&image, 1, 0, cv::Mat(), hist, 1, &histSize, 0);
	int top = hist.at<float>(histSize-1) + hist.at<float>(histSize-2) + hist.at<float>(histSize-3) ;
	cout << "Pixels is " << pixels << " Desired is " << .05 * pixels << " top 3 are " << top << std::endl;
	if ( top < peakMinCutoff)
		return 1;
	if ( top > peakMaxCutoff)
		return -1;

	if ( display) {
		cv::Mat histImage = cv::Mat::ones(200, 320, CV_8U)*255;
		cv::Mat dh;
		cv::normalize(hist, dh, 0, histImage.rows, cv::NORM_MINMAX, CV_32F);

		histImage = cv::Scalar::all(255);
		int binW = cvRound((double)histImage.cols/histSize);

		for( int i = 0; i < histSize; i++ )
			cv::rectangle( histImage, cv::Point(i*binW, histImage.rows),
				cv::Point((i+1)*binW, histImage.rows - cvRound(dh.at<float>(i))),
				cv::Scalar::all(0), -1, 8, 0 );
		cv::imshow("histogram", histImage);
		cv::waitKey(10000);
	}
	int lPeak = -1, rPeak = -1;

	/*
	 * Find the histogram bin just right of the left peak.
	 * use that to find first a min "white" value. Then an area where there is no value below that min.
	 */
	for ( int i = histSize-2; i > 0; i--) {
		if ( hist.at<int>(i) > peakMinCutoff)
			if ( hist.at<float>(i+1) > hist.at<float>(i)) {
				lPeak = i;
				break;
		}
	}
	if ( display) std::cout << "Peak degrades at " << lPeak << std::endl;
	if ( lPeak < minWhite)
		return 1;
	if ( lPeak >= maxWhite)
		return -1;
	return 0;
}
///////////
int main(int argc, char **argv)
{
	std::string s;
	s.append(argc > 1 ? argv[1] : "C:\\Data\\18-nov\\");
	s.append("*.png");
	bool quiet = ( argc > 2) ? ( std::string(argv[2]) == "-q") : false;
	if ( quiet) display = 0;
	// show the image
	if ( !quiet )
		cv::namedWindow("Rotated Image");

	std::vector<cv::String> fileNames;
	cv::glob(s, fileNames);

	for (int i=0; i<fileNames.size(); i++) {
		int key;

		std::cout << "File: " << fileNames[i] << endl;
		cv::Mat I1 = cv::imread(std::string(fileNames[i].c_str()), cv::IMREAD_GRAYSCALE);
		if (I1.rows<=0)
			continue;

		{
			int r = checkExposure( I1);
			cout << "Exposure should be " << r << std::endl;
			if (!quiet) {
				cv::resize(I1, I1, cv::Size(0,0), 2, 2, cv::INTER_LANCZOS4);
				cv::imshow("Barcode Image", I1);
				key = cv::waitKey(10);
				if ( key == 27)
					break;
			}

		}
	}
	cv::destroyAllWindows();

	return 0;
}
