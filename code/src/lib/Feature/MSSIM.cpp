#include "MSSIM.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

namespace feature
{
	MSSIM::MSSIM(
		std::shared_ptr<const cv::Mat> a,
		std::shared_ptr<const cv::Mat> b)
		:
		Interface(a, b)
	{
		// Taken from: https://docs.opencv.org/2.4/doc/tutorials/highgui/video-input-psnr-ssim/video-input-psnr-ssim.html
		
		const double C1 = 6.5025, C2 = 58.5225;
		/***************************** INITS **********************************/
		int d     = CV_32F;
		
		Mat I1, I2;
		a->convertTo(I1, d);           // cannot calculate on one byte large values
		b->convertTo(I2, d);
		
		Mat I2_2   = I2.mul(I2);        // I2^2
		Mat I1_2   = I1.mul(I1);        // I1^2
		Mat I1_I2  = I1.mul(I2);        // I1 * I2
		
		/***********************PRELIMINARY COMPUTING ******************************/
		
		Mat mu1, mu2;   //
		GaussianBlur(I1, mu1, Size(11, 11), 1.5);
		GaussianBlur(I2, mu2, Size(11, 11), 1.5);
		
		Mat mu1_2   =   mu1.mul(mu1);
		Mat mu2_2   =   mu2.mul(mu2);
		Mat mu1_mu2 =   mu1.mul(mu2);
		
		Mat sigma1_2, sigma2_2, sigma12;
		
		GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
		sigma1_2 -= mu1_2;
		
		GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
		sigma2_2 -= mu2_2;
		
		GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
		sigma12 -= mu1_mu2;
		
		///////////////////////////////// FORMULA ////////////////////////////////
		Mat t1, t2, t3;
		
		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
		
		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
		
		Mat ssim_map;
		divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;
		
		Scalar mssim = mean( ssim_map ); // mssim = average of ssim map

		// Store feature of structural similarity
		_features["mssim_b"] = mssim.val[0];
		_features["mssim_g"] = mssim.val[1];
		_features["mssim_r"] = mssim.val[2];
	}
}
