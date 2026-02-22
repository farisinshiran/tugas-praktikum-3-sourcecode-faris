#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

// -------------------------------------------------------
// Fungsi helper: hitung piksel putih pada mask (area terdeteksi)
// -------------------------------------------------------
int hitungPikselPutih(const cv::Mat& mask) {
    return cv::countNonZero(mask);
}

// -------------------------------------------------------
// Fungsi helper: gambar bounding box pada kontur yang ditemukan
// -------------------------------------------------------
void gambarBoundingBox(cv::Mat& gambar, const cv::Mat& mask,
                       const cv::Scalar& warnaBB, const std::string& label) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < 500) continue;  // abaikan noise kecil

        cv::Rect bb = cv::boundingRect(contours[i]);
        cv::rectangle(gambar, bb, warnaBB, 2);

        // Label di atas bounding box
        std::string teks = label + " #" + std::to_string(i + 1);
        cv::putText(gambar, teks,
                    cv::Point(bb.x, bb.y - 8),
                    cv::FONT_HERSHEY_SIMPLEX, 0.55, warnaBB, 2);
    }
}

int main(int argc, char** argv) {
    // -------------------------------------------------------
    // 1. Load gambar tomat
    // -------------------------------------------------------
    std::string imagePath = (argc > 1) ? argv[1] : "tomat.jpg";

    cv::Mat imgBGR = cv::imread(imagePath);
    if (imgBGR.empty()) {
        std::cerr << "[ERROR] Gambar tidak ditemukan: " << imagePath << std::endl;
        std::cerr << "Usage: ./sortir_tomat <path_to_image>" << std::endl;
        return -1;
    }

    std::cout << "[INFO] Gambar dimuat: " << imagePath
              << " (" << imgBGR.cols << "x" << imgBGR.rows << ")" << std::endl;

    // -------------------------------------------------------
    // 2. Convert BGR -> HSV
    // -------------------------------------------------------
    cv::Mat imgHSV;
    cv::cvtColor(imgBGR, imgHSV, cv::COLOR_BGR2HSV);

    // -------------------------------------------------------
    // 3. Definisi range warna tiap kategori kematangan
    //
    //    MERAH  (matang)        : H 0-10  ATAU H 170-180
    //    ORANYE (setengah matang): H 10-25
    //    KUNING (setengah matang): H 25-35
    //    HIJAU  (mentah)        : H 35-85
    // -------------------------------------------------------

    // --- Merah: dua range karena melingkar di ujung Hue ---
    cv::Mat maskMerah1, maskMerah2, maskMerah;
    cv::inRange(imgHSV, cv::Scalar(0,   80, 50), cv::Scalar(10,  255, 255), maskMerah1);
    cv::inRange(imgHSV, cv::Scalar(170, 80, 50), cv::Scalar(180, 255, 255), maskMerah2);
    cv::bitwise_or(maskMerah1, maskMerah2, maskMerah);  // gabung dua range

    // --- Oranye ---
    cv::Mat maskOranye;
    cv::inRange(imgHSV, cv::Scalar(10, 80, 50), cv::Scalar(25, 255, 255), maskOranye);

    // --- Kuning ---
    cv::Mat maskKuning;
    cv::inRange(imgHSV, cv::Scalar(25, 80, 50), cv::Scalar(35, 255, 255), maskKuning);

    // --- Hijau (mentah) ---
    cv::Mat maskHijau;
    cv::inRange(imgHSV, cv::Scalar(35, 40, 40), cv::Scalar(85, 255, 255), maskHijau);

    // -------------------------------------------------------
    // 4. Haluskan semua mask dengan morfologi
    // -------------------------------------------------------
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(7, 7));
    auto haluskan = [&](cv::Mat& m) {
        cv::morphologyEx(m, m, cv::MORPH_OPEN,  kernel);
        cv::morphologyEx(m, m, cv::MORPH_CLOSE, kernel);
    };
    haluskan(maskMerah);
    haluskan(maskOranye);
    haluskan(maskKuning);
    haluskan(maskHijau);

    // -------------------------------------------------------
    // 5. Hitung piksel putih (luas area tiap warna)
    // -------------------------------------------------------
    int pixMerah  = hitungPikselPutih(maskMerah);
    int pixOranye = hitungPikselPutih(maskOranye);
    int pixKuning = hitungPikselPutih(maskKuning);
    int pixHijau  = hitungPikselPutih(maskHijau);
    int pixTotal  = pixMerah + pixOranye + pixKuning + pixHijau;

    std::cout << "\n========================================" << std::endl;
    std::cout << "  HASIL ANALISIS KEMATANGAN TOMAT" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Merah   (Matang)        : " << pixMerah  << " piksel" << std::endl;
    std::cout << "  Oranye  (Setengah Matang): " << pixOranye << " piksel" << std::endl;
    std::cout << "  Kuning  (Setengah Matang): " << pixKuning << " piksel" << std::endl;
    std::cout << "  Hijau   (Mentah)         : " << pixHijau  << " piksel" << std::endl;
    std::cout << "  Total Terdeteksi         : " << pixTotal  << " piksel" << std::endl;

    if (pixTotal > 0) {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "  Dominasi Merah   : "
                  << std::fixed << std::setprecision(1)
                  << (100.0 * pixMerah  / pixTotal) << "%" << std::endl;
        std::cout << "  Dominasi Hijau   : "
                  << (100.0 * pixHijau  / pixTotal) << "%" << std::endl;
        std::cout << "  Dominasi Oranye  : "
                  << (100.0 * pixOranye / pixTotal) << "%" << std::endl;
        std::cout << "  Dominasi Kuning  : "
                  << (100.0 * pixKuning / pixTotal) << "%" << std::endl;
    }
    std::cout << "========================================\n" << std::endl;

    // -------------------------------------------------------
    // 6. Buat gambar output dengan bounding box berwarna
    // -------------------------------------------------------
    cv::Mat imgBoundingBox = imgBGR.clone();
    gambarBoundingBox(imgBoundingBox, maskMerah,  cv::Scalar(0, 0, 255),   "Matang");
    gambarBoundingBox(imgBoundingBox, maskOranye, cv::Scalar(0, 128, 255), "1/2 Matang");
    gambarBoundingBox(imgBoundingBox, maskKuning, cv::Scalar(0, 255, 255), "1/2 Matang");
    gambarBoundingBox(imgBoundingBox, maskHijau,  cv::Scalar(0, 255, 0),   "Mentah");

    // -------------------------------------------------------
    // 7. Buat window output untuk tiap kategori
    //    (extract piksel asli sesuai mask)
    // -------------------------------------------------------
    cv::Mat outMerah, outHijau, outOranye, outKuning;
    cv::bitwise_and(imgBGR, imgBGR, outMerah,  maskMerah);
    cv::bitwise_and(imgBGR, imgBGR, outHijau,  maskHijau);
    cv::bitwise_and(imgBGR, imgBGR, outOranye, maskOranye);
    cv::bitwise_and(imgBGR, imgBGR, outKuning, maskKuning);

    // -------------------------------------------------------
    // 8. Simpan semua output
    // -------------------------------------------------------
    cv::imwrite("output_tomat_bounding_box.jpg", imgBoundingBox);
    cv::imwrite("output_tomat_merah.jpg",   outMerah);
    cv::imwrite("output_tomat_hijau.jpg",   outHijau);
    cv::imwrite("output_tomat_oranye.jpg",  outOranye);
    cv::imwrite("output_tomat_kuning.jpg",  outKuning);
    cv::imwrite("output_mask_merah.jpg",    maskMerah);
    cv::imwrite("output_mask_hijau.jpg",    maskHijau);

    std::cout << "[INFO] Semua output gambar berhasil disimpan." << std::endl;

    // -------------------------------------------------------
    // 9. Tampilkan semua window
    //    Window 1 = Tomat Hijau (mentah)
    //    Window 2 = Tomat Merah (matang)
    //    + window tambahan
    // -------------------------------------------------------
    cv::imshow("Original", imgBGR);
    cv::imshow("Window 1 - Tomat HIJAU (Mentah)",          outHijau);
    cv::imshow("Window 2 - Tomat MERAH (Matang)",          outMerah);
    cv::imshow("Window 3 - Tomat ORANYE (Setengah Matang)",outOranye);
    cv::imshow("Window 4 - Tomat KUNING (Setengah Matang)",outKuning);
    cv::imshow("Bounding Box Semua Kategori",               imgBoundingBox);
    cv::imshow("Mask Merah", maskMerah);
    cv::imshow("Mask Hijau", maskHijau);

    std::cout << "[INFO] Tekan sembarang tombol untuk menutup..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}