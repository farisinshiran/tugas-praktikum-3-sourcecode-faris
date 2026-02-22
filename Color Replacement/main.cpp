#include <opencv2/opencv.hpp>
#include <iostream>

int main(int argc, char** argv) {
    // -------------------------------------------------------
    // 1. Load gambar (gunakan argumen CLI atau gambar default)
    // -------------------------------------------------------
    std::string imagePath = (argc > 1) ? argv[1] : "input.jpg";

    cv::Mat imgBGR = cv::imread(imagePath);
    if (imgBGR.empty()) {
        std::cerr << "[ERROR] Gambar tidak ditemukan: " << imagePath << std::endl;
        std::cerr << "Usage: ./color_replacement <path_to_image>" << std::endl;
        return -1;
    }

    std::cout << "[INFO] Gambar berhasil dimuat: " << imagePath << std::endl;
    std::cout << "[INFO] Ukuran: " << imgBGR.cols << "x" << imgBGR.rows << std::endl;

    // -------------------------------------------------------
    // 2. Convert BGR -> HSV
    // -------------------------------------------------------
    cv::Mat imgHSV;
    cv::cvtColor(imgBGR, imgHSV, cv::COLOR_BGR2HSV);

    // -------------------------------------------------------
    // 3. Buat Mask untuk warna Biru (Hue: 100 - 130)
    //    OpenCV: Hue range = 0-179 (setengah dari 0-360)
    //    Biru  -> H: 100-130, S: 50-255, V: 50-255
    // -------------------------------------------------------
    cv::Scalar lowerBlue(100, 50, 50);
    cv::Scalar upperBlue(130, 255, 255);

    cv::Mat mask;
    cv::inRange(imgHSV, lowerBlue, upperBlue, mask);

    std::cout << "[INFO] Mask biru berhasil dibuat." << std::endl;

    // -------------------------------------------------------
    // 4. Ganti Hue dari Biru (100-130) -> Hijau (60)
    //    Hanya ubah channel H (index 0)
    //    Pertahankan S (index 1) dan V (index 2)
    // -------------------------------------------------------
    // Split HSV menjadi 3 channel terpisah
    std::vector<cv::Mat> channels;
    cv::split(imgHSV, channels);
    // channels[0] = Hue
    // channels[1] = Saturation
    // channels[2] = Value (kecerahan)

    // Pada piksel yang terdeteksi mask, set Hue = 60 (Hijau)
    channels[0].setTo(60, mask);

    // Gabungkan kembali channel
    cv::Mat imgHSV_result;
    cv::merge(channels, imgHSV_result);

    // -------------------------------------------------------
    // 5. Convert HSV -> BGR untuk ditampilkan & disimpan
    // -------------------------------------------------------
    cv::Mat imgResult;
    cv::cvtColor(imgHSV_result, imgResult, cv::COLOR_HSV2BGR);

    std::cout << "[INFO] Konversi warna selesai." << std::endl;

    // -------------------------------------------------------
    // 6. Simpan hasil & tampilkan perbandingan
    // -------------------------------------------------------
    std::string outputPath = "output_color_replaced.jpg";
    cv::imwrite(outputPath, imgResult);
    std::cout << "[INFO] Hasil disimpan ke: " << outputPath << std::endl;

    // Simpan juga mask untuk referensi
    cv::imwrite("output_mask_biru.jpg", mask);
    std::cout << "[INFO] Mask disimpan ke: output_mask_biru.jpg" << std::endl;

    // Tampilkan jendela (opsional, butuh GUI)
    cv::imshow("Original (BGR)", imgBGR);
    cv::imshow("Mask Biru", mask);
    cv::imshow("Hasil: Biru -> Hijau", imgResult);

    std::cout << "[INFO] Tekan sembarang tombol untuk menutup jendela..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}