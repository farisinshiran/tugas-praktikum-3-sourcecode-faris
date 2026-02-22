#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // Membaca gambar dengan path yang lebih jelas
    // Coba beberapa kemungkinan lokasi file
    Mat image = imread("keranjang_merah.jpg");
    
    // Pengecekan penting: apakah gambar berhasil dibaca?
    if (image.empty()) {
        cout << "Error: Tidak dapat membaca file keranjang_merah.jpg" << endl;
        cout << "Pastikan file berada di folder yang sama dengan executable" << endl;
        cout << "Atau gunakan absolute path" << endl;
        return -1;  // Keluar dari program dengan error code
    }
    
    // Jika sampai sini, berarti gambar berhasil dibaca
    cout << "Gambar berhasil dibaca!" << endl;
    cout << "Ukuran: " << image.cols << "x" << image.rows << endl;
    
    // Lanjutkan dengan proses splitting channel
    Mat bgr[3];   // Array untuk menyimpan channel Blue, Green, Red
    split(image, bgr);  // Memisahkan channel warna
    
    // Tampilkan hasil
    imshow("Original", image);
    imshow("Blue Channel", bgr[0]);
    imshow("Green Channel", bgr[1]);
    imshow("Red Channel", bgr[2]);
    
    waitKey(0);  // Tunggu sampai user menekan tombol
    return 0;
}