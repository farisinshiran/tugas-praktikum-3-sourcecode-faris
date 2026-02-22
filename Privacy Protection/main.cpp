#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    // -------------------------------------------------------
    // 1. Load gambar input
    // -------------------------------------------------------
    std::string imagePath = (argc > 1) ? argv[1] : "input.jpg";

    cv::Mat imgBGR = cv::imread(imagePath);
    if (imgBGR.empty()) {
        std::cerr << "[ERROR] Gambar tidak ditemukan: " << imagePath << std::endl;
        std::cerr << "Usage: ./privacy_blur <path_to_image>" << std::endl;
        return -1;
    }

    std::cout << "[INFO] Gambar dimuat: " << imagePath
              << " (" << imgBGR.cols << "x" << imgBGR.rows << ")" << std::endl;

    // -------------------------------------------------------
    // 2. Convert BGR -> YCrCb
    //    Y  = Luminance (kecerahan)
    //    Cr = Red-Chrominance
    //    Cb = Blue-Chrominance
    //
    //    Range kulit manusia pada YCrCb (penelitian Kovac et al.):
    //    Y  : 0   - 255  (tidak dibatasi)
    //    Cr : 133 - 173
    //    Cb : 77  - 127
    // -------------------------------------------------------
    cv::Mat imgYCrCb;
    cv::cvtColor(imgBGR, imgYCrCb, cv::COLOR_BGR2YCrCb);

    // -------------------------------------------------------
    // 3. Buat Mask area kulit menggunakan inRange pada YCrCb
    // -------------------------------------------------------
    cv::Scalar lowerSkin(0,   133, 77);
    cv::Scalar upperSkin(255, 173, 127);

    cv::Mat maskKulit;
    cv::inRange(imgYCrCb, lowerSkin, upperSkin, maskKulit);

    std::cout << "[INFO] Deteksi area kulit (YCrCb) selesai." << std::endl;

    // -------------------------------------------------------
    // 4. Haluskan mask dengan operasi morfologi
    //    OPEN  -> hapus noise titik-titik kecil
    //    CLOSE -> tutup lubang kecil di dalam area kulit
    //    DILATE -> perluas sedikit agar seluruh area wajah tertutup
    // -------------------------------------------------------
    cv::Mat kernel5 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::Mat kernel15= cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15, 15));

    cv::morphologyEx(maskKulit, maskKulit, cv::MORPH_OPEN,  kernel5);
    cv::morphologyEx(maskKulit, maskKulit, cv::MORPH_CLOSE, kernel15);
    cv::dilate(maskKulit, maskKulit, kernel5, cv::Point(-1,-1), 2);

    // -------------------------------------------------------
    // 5. Gaussian Blur pada SELURUH gambar (kernel besar)
    //    Kernel harus bilangan ganjil; semakin besar = semakin buram
    // -------------------------------------------------------
    cv::Mat imgBlurred;
    int kernelSize = 51;  // ubah ke 99 untuk blur lebih ekstrem
    cv::GaussianBlur(imgBGR, imgBlurred, cv::Size(kernelSize, kernelSize), 0);

    std::cout << "[INFO] Gaussian Blur diterapkan (kernel " 
              << kernelSize << "x" << kernelSize << ")." << std::endl;

    // -------------------------------------------------------
    // 6. Gabungkan: area kulit = BLUR, area lain = TAJAM
    //
    //    Cara kerja:
    //    - maskKulit        : area kulit = 255 (putih)
    //    - maskBackground   : area background = 255 (putih) -> invert
    //    - ambil piksel blur  hanya di area kulit
    //    - ambil piksel tajam hanya di area background
    //    - gabung dengan bitwise_or
    // -------------------------------------------------------
    cv::Mat maskBackground;
    cv::bitwise_not(maskKulit, maskBackground);

    cv::Mat areaBlur, areaTajam;
    cv::bitwise_and(imgBlurred, imgBlurred, areaBlur,  maskKulit);
    cv::bitwise_and(imgBGR,     imgBGR,     areaTajam, maskBackground);

    cv::Mat imgResult;
    cv::bitwise_or(areaBlur, areaTajam, imgResult);

    std::cout << "[INFO] Penggabungan area blur + tajam selesai." << std::endl;

    // -------------------------------------------------------
    // 7. (Opsional) Gambar kontur area kulit di atas hasil
    //    untuk visualisasi area yang dideteksi
    // -------------------------------------------------------
    cv::Mat imgResultAnnotated = imgResult.clone();
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(maskKulit, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    int jumlahArea = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < 1000) continue;  // abaikan area terlalu kecil

        jumlahArea++;
        cv::Rect bb = cv::boundingRect(contours[i]);

        // Kotak merah menunjukkan area yang diblur
        cv::rectangle(imgResultAnnotated, bb, cv::Scalar(0, 0, 255), 2);
        cv::putText(imgResultAnnotated,
                    "Wajah/Kulit #" + std::to_string(jumlahArea),
                    cv::Point(bb.x, bb.y - 8),
                    cv::FONT_HERSHEY_SIMPLEX, 0.55,
                    cv::Scalar(0, 0, 255), 2);
    }

    // Statistik area kulit
    int pixKulit = cv::countNonZero(maskKulit);
    int pixTotal = imgBGR.rows * imgBGR.cols;

    std::cout << "\n========================================" << std::endl;
    std::cout << "  HASIL SENSOR PRIVASI OTOMATIS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Area kulit terdeteksi : " << pixKulit << " piksel" << std::endl;
    std::cout << "  Total piksel gambar   : " << pixTotal << " piksel" << std::endl;
    std::cout << "  Persentase area kulit : " << std::fixed << std::setprecision(2)
              << (100.0 * pixKulit / pixTotal) << "%" << std::endl;
    std::cout << "  Jumlah area wajah/kulit: " << jumlahArea << std::endl;
    std::cout << "  Kernel Gaussian Blur   : " << kernelSize
              << "x" << kernelSize << std::endl;
    std::cout << "========================================\n" << std::endl;

    // -------------------------------------------------------
    // 8. Simpan semua output
    // -------------------------------------------------------
    cv::imwrite("output_privacy_blur.jpg",       imgResult);
    cv::imwrite("output_privacy_annotated.jpg",  imgResultAnnotated);
    cv::imwrite("output_mask_kulit.jpg",         maskKulit);
    cv::imwrite("output_blur_full.jpg",          imgBlurred);

    std::cout << "[INFO] Semua output berhasil disimpan:" << std::endl;
    std::cout << "       - output_privacy_blur.jpg       (hasil utama)" << std::endl;
    std::cout << "       - output_privacy_annotated.jpg  (dengan bounding box)" << std::endl;
    std::cout << "       - output_mask_kulit.jpg         (mask area kulit)" << std::endl;
    std::cout << "       - output_blur_full.jpg          (full blur referensi)" << std::endl;

    // -------------------------------------------------------
    // 9. Tampilkan semua window
    // -------------------------------------------------------
    cv::imshow("1. Original (Asli)",               imgBGR);
    cv::imshow("2. Mask Area Kulit (YCrCb)",        maskKulit);
    cv::imshow("3. Full Gaussian Blur (referensi)", imgBlurred);
    cv::imshow("4. Hasil Privacy Blur",             imgResult);
    cv::imshow("5. Anotasi Bounding Box",           imgResultAnnotated);

    std::cout << "[INFO] Tekan sembarang tombol untuk menutup..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}