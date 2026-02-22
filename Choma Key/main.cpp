#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    // -------------------------------------------------------
    // 1. Load gambar presenter (blue screen) & background baru
    // -------------------------------------------------------
    std::string personPath = (argc > 1) ? argv[1] : "presenter.jpg";
    std::string bgPath     = (argc > 2) ? argv[2] : "background.jpg";

    cv::Mat imgPerson = cv::imread(personPath);
    cv::Mat imgBG     = cv::imread(bgPath);

    if (imgPerson.empty()) {
        std::cerr << "[ERROR] Gambar presenter tidak ditemukan: " << personPath << std::endl;
        return -1;
    }
    if (imgBG.empty()) {
        std::cerr << "[ERROR] Gambar background tidak ditemukan: " << bgPath << std::endl;
        return -1;
    }

    std::cout << "[INFO] Presenter : " << personPath
              << " (" << imgPerson.cols << "x" << imgPerson.rows << ")" << std::endl;
    std::cout << "[INFO] Background: " << bgPath
              << " (" << imgBG.cols << "x" << imgBG.rows << ")" << std::endl;

    // -------------------------------------------------------
    // 2. Seragamkan ukuran background dengan gambar presenter
    // -------------------------------------------------------
    cv::Mat imgBGResized;
    cv::resize(imgBG, imgBGResized, imgPerson.size());

    // -------------------------------------------------------
    // 3. Convert gambar presenter ke HSV
    // -------------------------------------------------------
    cv::Mat imgHSV;
    cv::cvtColor(imgPerson, imgHSV, cv::COLOR_BGR2HSV);

    // -------------------------------------------------------
    // 4. Buat Mask area BIRU (latar belakang blue screen)
    //    Hue: 90-130  |  S: 80-255  |  V: 50-255
    //    (range S & V dibatasi agar tidak mendeteksi
    //     warna biru sangat gelap atau sangat pucat)
    // -------------------------------------------------------
    cv::Scalar lowerBlue(90, 80, 50);
    cv::Scalar upperBlue(130, 255, 255);

    cv::Mat maskBlue;  // area biru = putih (255)
    cv::inRange(imgHSV, lowerBlue, upperBlue, maskBlue);

    // -------------------------------------------------------
    // 5. Haluskan mask dengan operasi morfologi
    //    (menghilangkan noise kecil & menutup lubang kecil)
    // -------------------------------------------------------
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(maskBlue, maskBlue, cv::MORPH_OPEN,  kernel);  // hapus noise kecil
    cv::morphologyEx(maskBlue, maskBlue, cv::MORPH_CLOSE, kernel);  // tutup lubang kecil

    // -------------------------------------------------------
    // 6. Invert Mask -> area ORANG = putih (255)
    // -------------------------------------------------------
    cv::Mat maskPerson;
    cv::bitwise_not(maskBlue, maskPerson);

    // -------------------------------------------------------
    // 7. Ekstrak area ORANG dari gambar presenter
    //    bitwise_and: hanya piksel di mana mask = 255 yang dipertahankan
    // -------------------------------------------------------
    cv::Mat personOnly;
    cv::bitwise_and(imgPerson, imgPerson, personOnly, maskPerson);

    // -------------------------------------------------------
    // 8. Ekstrak area BACKGROUND baru (area yang bukan orang)
    //    Gunakan maskBlue sebagai area tempat background tampil
    // -------------------------------------------------------
    cv::Mat bgOnly;
    cv::bitwise_and(imgBGResized, imgBGResized, bgOnly, maskBlue);

    // -------------------------------------------------------
    // 9. Gabungkan orang + background baru dengan bitwise_or
    // -------------------------------------------------------
    cv::Mat result;
    cv::bitwise_or(personOnly, bgOnly, result);

    // -------------------------------------------------------
    // 10. Simpan & tampilkan hasil
    // -------------------------------------------------------
    cv::imwrite("output_chroma_key.jpg",  result);
    cv::imwrite("output_mask_biru.jpg",   maskBlue);
    cv::imwrite("output_mask_orang.jpg",  maskPerson);

    std::cout << "[INFO] Hasil disimpan ke: output_chroma_key.jpg" << std::endl;

    cv::imshow("1. Presenter (Original)", imgPerson);
    cv::imshow("2. Background Baru",      imgBGResized);
    cv::imshow("3. Mask Biru (Blue Screen)", maskBlue);
    cv::imshow("4. Mask Orang (Inverted)",   maskPerson);
    cv::imshow("5. Hasil Chroma Key",        result);

    std::cout << "[INFO] Tekan sembarang tombol untuk menutup..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}