#include <opencv2/opencv.hpp>
#include <iostream>
// Struct untuk menyimpan parameter HSV
struct HSVParams
{
    int h_min = 0;
    int h_max = 179;
    int s_min = 0;
    int s_max = 255;
    int v_min = 0;
    int v_max = 255;
};
// Global variable
HSVParams params;
// Callback function
void onTrackbar(int, void *) {}
void setPresetColor(const std::string &color)
{
    if (color == "red")
    {
        params.h_min = 0;
        params.h_max = 10;
        params.s_min = 100;
        params.s_max = 255;
        params.v_min = 100;
        params.v_max = 255;
        std::cout << "Preset: MERAH\n";
    }
    else if (color == "blue")
    {
        params.h_min = 100;
        params.h_max = 130;
        params.s_min = 50;
        params.s_max = 255;
        params.v_min = 50;
        params.v_max = 255;
        std::cout << "Preset: BIRU\n";
    }
    else if (color == "green")
    {
        params.h_min = 40;
        params.h_max = 80;
        params.s_min = 50;
        params.s_max = 255;
        params.v_min = 50;
        params.v_max = 255;
        std::cout << "Preset: HIJAU\n";
    }
    else if (color == "yellow")
    {
        params.h_min = 20;
        params.h_max = 35;
        params.s_min = 100;
        params.s_max = 255;
        params.v_min = 100;
        params.v_max = 255;
        std::cout << "Preset: KUNING\n";
    }
    cv::setTrackbarPos("Hue Min", "Control Panel", params.h_min);
    cv::setTrackbarPos("Hue Max", "Control Panel", params.h_max);
    cv::setTrackbarPos("Sat Min", "Control Panel", params.s_min);
    cv::setTrackbarPos("Sat Max", "Control Panel", params.s_max);
    cv::setTrackbarPos("Val Min", "Control Panel", params.v_min);
    cv::setTrackbarPos("Val Max", "Control Panel", params.v_max);
}
void printHelp()
{
    std::cout << "\n=== COLOR TRACKER ===\n";
    std::cout << "q - Keluar\n";
    std::cout << "s - Save parameter\n";
    std::cout << "r/b/g/y - Preset Merah/Biru/Hijau/Kuning\n\n";
}
int main()
{
    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cout << "Error: Kamera tidak bisa dibuka!\n";
        return -1;
    }
    cv::namedWindow("Original");
    cv::namedWindow("Mask");
    cv::namedWindow("Hasil Deteksi");
    cv::namedWindow("Control Panel");
    cv::resizeWindow("Control Panel", 400, 300);
    cv::createTrackbar("Hue Min", "Control Panel", &params.h_min, 179, onTrackbar);
    cv::createTrackbar("Hue Max", "Control Panel", &params.h_max, 179, onTrackbar);
    cv::createTrackbar("Sat Min", "Control Panel", &params.s_min, 255, onTrackbar);
    cv::createTrackbar("Sat Max", "Control Panel", &params.s_max, 255, onTrackbar);
    cv::createTrackbar("Val Min", "Control Panel", &params.v_min, 255, onTrackbar);
    cv::createTrackbar("Val Max", "Control Panel", &params.v_max, 255, onTrackbar);
    printHelp();
    cv::Mat frame, hsv, mask, result;
    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
        cv::Scalar lower(params.h_min, params.s_min, params.v_min);
        cv::Scalar upper(params.h_max, params.s_max, params.v_max);
        cv::inRange(hsv, lower, upper, mask);
        // Morphological operations
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
        cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
        cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);
        result = cv::Mat::zeros(frame.size(), frame.type());
        cv::bitwise_and(frame, frame, result, mask);
        // Info text
        int detectedPixels = cv::countNonZero(mask);
        double percentage = (detectedPixels * 100.0) / (mask.rows * mask.cols);
        std::string info = cv::format("Detected: %.2f%%", percentage);
        cv::putText(frame, info, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX,
                    0.7, cv::Scalar(0, 255, 0), 2);
        std::string hsvInfo = cv::format("H:[%d,%d] S:[%d,%d] V:[%d,%d]",
                                         params.h_min, params.h_max,
                                         params.s_min, params.s_max,
                                         params.v_min, params.v_max);
        cv::putText(frame, hsvInfo, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX,
                    0.5, cv::Scalar(255, 255, 0), 1);
        cv::imshow("Original", frame);
        cv::imshow("Mask", mask);
        cv::imshow("Hasil Deteksi", result);
        char key = cv::waitKey(1);
        if (key == 'q' || key == 'Q')
            break;
        else if (key == 's' || key == 'S')
        {
            std::cout << "\n=== PARAMETER HSV ===\n";
            std::cout << "Scalar lower(" << params.h_min << ", " << params.s_min << ", "
                      << params.v_min << ");\n";
            std::cout << "Scalar upper(" << params.h_max << ", " << params.s_max << ", "
                      << params.v_max << ");\n\n";
        }
        else if (key == 'r')
            setPresetColor("red");
        else if (key == 'b')
            setPresetColor("blue");
        else if (key == 'g')
            setPresetColor("green");
        else if (key == 'y')
            setPresetColor("yellow");
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}