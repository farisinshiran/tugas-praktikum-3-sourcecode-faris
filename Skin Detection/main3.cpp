#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
    Mat src = imread("bola_merah.jpg");
    Mat hsv, mask1, mask2, mask_final, result;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    // Rentang Merah 1 (0 - 10 derajat)
    // Scalar(H_min, S_min, V_min) -> Scalar(H_max, S_max, V_max)
    inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), mask1);
    // Rentang Merah 2 (170 - 180 derajat) - Mengatasi wrap-around warna
    // merah
        inRange(hsv, Scalar(170, 100, 100), Scalar(180, 255, 255), mask2);
    // Gabungkan kedua mask (operasi OR)
    bitwise_or(mask1, mask2, mask_final);
    // Terapkan mask ke gambar asli untuk melihat hasil segmentasi
    // Set background jadi hitam, objek tetap berwarna
    result = Mat::zeros(src.size(), src.type());
    src.copyTo(result, mask_final);
    imshow("Original", src);
    imshow("Mask (Binary)", mask_final); // Putih = Terdeteksi, Hitam =
    // Background
        imshow("Hasil Deteksi", result);
    waitKey(0);
    return 0;
}