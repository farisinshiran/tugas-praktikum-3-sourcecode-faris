#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
int main()
{
    Mat src = imread("keranjang_merah.jpg");
    Mat hsv, mask1, mask2, mask_final, result;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    // Rentang Merah 1 (0-10 derajat)
    inRange(hsv, Scalar(0, 100, 100),
            Scalar(10, 255, 255), mask1);
    // Rentang Merah 2 (170-180 derajat)
    inRange(hsv, Scalar(170, 100, 100),
            Scalar(180, 255, 255), mask2);
    bitwise_or(mask1, mask2, mask_final);
    result = Mat::zeros(src.size(), src.type());
    src.copyTo(result, mask_final);
    imshow("Original", src);
    imshow("Mask (Binary)", mask_final);
    imshow("Hasil Deteksi", result);
    waitKey(0);
    return 0;
}