// Pustaka standar yang digunakan
#include <iostream> // Untuk operasi input/output (misalnya, std::cout, std::cerr)
#include <fstream>  // Untuk operasi file (misalnya, std::ofstream untuk menulis file)
#include <vector>   // Untuk menggunakan struktur data std::vector (array dinamis)
#include <iomanip>  // Untuk manipulator output (misalnya, std::fixed, std::setprecision)
#include <random>   // Untuk fungsionalitas pembuatan angka acak

// Struktur DataParams digunakan untuk menyimpan satu set parameter input
// yang akan digunakan untuk satu kasus uji dalam program Drone.cpp.
struct DataParams {
    double c1;           // Koefisien terkait gaya hambat parasitik drone
    double c2;           // Koefisien terkait gaya hambat terinduksi drone
    double v0_initial;   // Tebakan awal untuk kecepatan optimal (v0) dalam m/s
    double tolerance;    // Toleransi error untuk kriteria konvergensi metode Newton-Raphson
    int max_iterations;  // Jumlah iterasi maksimum yang diizinkan untuk metode Newton-Raphson
};

// Fungsi utama program
int main() {
    // Nama file output tempat data sintetis akan disimpan
    std::string output_filename = "synthetic_data.txt";
    // Membuat objek stream file output
    std::ofstream outfile(output_filename);

    // Memeriksa apakah file berhasil dibuka untuk ditulis
    if (!outfile.is_open()) {
        std::cerr << "Error: Tidak dapat membuka file output: " << output_filename << std::endl;
        return 1; // Mengembalikan kode error jika file tidak dapat dibuka
    }

    std::cout << "Menghasilkan data sintetis ke " << output_filename << "..." << std::endl;

    // Mendefinisikan beberapa set parameter secara manual untuk kasus uji awal
    // Setiap baris merepresentasikan satu set data: {c1, c2, v0_initial, tolerance, max_iterations}
    std::vector<DataParams> data_sets = {
        {0.1, 100.0, 5.0, 1e-6, 100},   // Kasus uji standar
        {0.05, 50.0, 5.0, 1e-6, 100},  // Nilai c1 dan c2 lebih kecil
        {0.2, 200.0, 10.0, 1e-7, 150}, // Nilai c1 dan c2 lebih besar, toleransi lebih ketat
        {0.1, 50.0, 1.0, 1e-5, 50},    // Tebakan awal (v0) yang berbeda, toleransi lebih longgar
        {0.15, 120.0, 7.0, 1e-6, 100},
        // Kasus uji tambahan untuk variasi yang lebih beragam
        {0.08, 80.0, 3.0, 1e-6, 120},   // Variasi c1, c2, v0
        {0.12, 150.0, 12.0, 1e-7, 200}, // c2 lebih tinggi, v0 lebih tinggi, iterasi maks lebih banyak
        {0.03, 30.0, 2.0, 1e-5, 80},    // Nilai c yang rendah
        {0.25, 250.0, 8.0, 1e-6, 100},
        {0.1, 100.0, 1.0, 1e-6, 100},   // c1,c2 sama dengan kasus pertama, v0 berbeda
        {0.1, 100.0, 15.0, 1e-6, 100}   // c1,c2 sama dengan kasus pertama, v0 berbeda lagi
    };
    
    // Persiapan untuk menghasilkan beberapa variasi data secara acak
    std::mt19937 rng(std::random_device{}()); // Inisialisasi generator angka acak Mersenne Twister
    // Distribusi untuk koefisien c1 (antara 0.01 dan 0.3)
    std::uniform_real_distribution<double> c_dist(0.01, 0.3);
    // Distribusi untuk tebakan awal v0 (antara 1.0 m/s dan 20.0 m/s)
    std::uniform_real_distribution<double> v0_dist(1.0, 20.0);

    // Menambahkan beberapa kasus uji yang dihasilkan secara acak
    // Loop ini akan berjalan 5 kali, menambahkan 5 set data acak
    for (int i = 0; i < 5; ++i) {
        double rand_c1 = c_dist(rng); // Menghasilkan c1 acak
        // Menghasilkan c2 acak yang proporsional dengan c1 untuk menjaga rasio yang masuk akal
        // Faktor pengali antara 500 dan 1500 dipilih secara acak
        double rand_c2 = rand_c1 * std::uniform_real_distribution<double>(500, 1500)(rng); 
        double rand_v0 = v0_dist(rng); // Menghasilkan v0 acak
        // Menambahkan set data acak ke dalam vektor data_sets
        // Toleransi dan iterasi maksimum diatur ke nilai default (1e-6 dan 100)
        data_sets.push_back({rand_c1, rand_c2, rand_v0, 1e-6, 100});
    }

    // Mengatur presisi angka desimal saat menulis ke file
    // std::fixed memastikan notasi titik tetap (bukan saintifik)
    // std::setprecision(8) mengatur jumlah angka di belakang koma menjadi 8
    outfile << std::fixed << std::setprecision(8);

    // Menulis setiap set parameter dari vektor data_sets ke file output
    // Format per baris: c1 c2 v0_initial tolerance max_iterations
    for (const auto& params : data_sets) {
        outfile << params.c1 << " "
                << params.c2 << " "
                << params.v0_initial << " "
                << params.tolerance << " "
                << params.max_iterations << std::endl;
    }

    // Menutup file setelah selesai menulis
    outfile.close();
    std::cout << "Data sintetis berhasil ditulis ke " << output_filename << "." << std::endl;
    std::cout << data_sets.size() << " set data telah dihasilkan." << std::endl;

    return 0; // Mengindikasikan program berakhir dengan sukses
}
