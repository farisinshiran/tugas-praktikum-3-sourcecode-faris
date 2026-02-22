#include <iostream>
#include <opencv2/opencv.hpp>

int main () {
    cv::Mat img = cv::imread("img.jpg");
    if (img.empty()) {
        std::cerr << "Error: Could not load image." << std::endl;
        return -1;
    }

    cv::Mat gray, hsv, lab;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(img, lab, cv::COLOR_BGR2Lab);

    cv::imshow("gray_image.jpg", gray);
    cv::imshow("hsv_image.jpg", hsv);
    cv::imshow("lab_image.jpg", lab);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;

}