#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>
using namespace cv;
using namespace std;
int main()
{
    VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;
    Mat background, frame, hsv, mask1, mask2, result;
    this_thread::sleep_for(chrono::seconds(2));
    cout << "Mengambil background... MINGGIR!" << endl;
    for (int i = 0; i < 60; i++)
    {
        cap >> background;
        if (i % 10 == 0)
            cout << "." << flush;
    }
    cout << "\nBackground tersimpan!" << endl;
    flip(background, background, 1);
    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;
        flip(frame, frame, 1);
        cvtColor(frame, hsv, COLOR_BGR2HSV);
        // Deteksi warna Biru
        Scalar lower_blue(90, 120, 70);
        Scalar upper_blue(130, 255, 255);
        inRange(hsv, lower_blue, upper_blue, mask1);
        // Morphology
        morphologyEx(mask1, mask1, MORPH_OPEN,
                     getStructuringElement(MORPH_RECT, Size(3, 3)));
        morphologyEx(mask1, mask1, MORPH_DILATE,
                     getStructuringElement(MORPH_RECT, Size(3, 3)));
        bitwise_not(mask1, mask2);
        Mat res1, res2, final_output;
        bitwise_and(background, background, res1, mask1);
        bitwise_and(frame, frame, res2, mask2);
        addWeighted(res1, 1, res2, 1, 0, final_output);
        imshow("Invisibility Cloak", final_output);
        if (waitKey(1) == 'q')
            break;
    }

    return 0;
}