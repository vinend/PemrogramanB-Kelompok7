// Pustaka standar yang digunakan
#include <iostream> // Untuk operasi input/output (misalnya, std::cout, std::cerr)
#include <cmath>    // Untuk fungsi matematika (misalnya, std::abs, std::pow)
#include <iomanip>  // Untuk manipulator output (misalnya, std::setw, std::fixed, std::setprecision)
#include <vector>   // Untuk menggunakan struktur data std::vector (array dinamis)
#include <string>   // Untuk menggunakan std::string
#include <fstream>  // Untuk operasi file (misalnya, std::ifstream untuk membaca file)
#include <sstream>  // Untuk parsing string (misalnya, std::istringstream)

// Fungsi f_drone(v, c1, c2)
// Menghitung nilai fungsi f(v) = 2*c1*v - 2*c2*v^(-3).
// Fungsi ini merepresentasikan turunan dari (Daya/Kecepatan) terhadap kecepatan (v),
// yang disetarakan dengan nol untuk mencari kecepatan yang memaksimalkan jangkauan terbang drone.
// Parameter:
//   v  (double): Kecepatan drone (m/s).
//   c1 (double): Koefisien terkait gaya hambat parasitik drone.
//   c2 (double): Koefisien terkait gaya hambat terinduksi drone.
// Mengembalikan (double): Nilai f(v).
double f_drone(double v, double c1, double c2) {
    // Memastikan v positif untuk menghindari pembagian dengan nol atau masalah dengan v negatif pada pow(v, -3).
    // Menggunakan batas bawah kecil (1e-9) sebagai pengganti perbandingan langsung dengan nol.
    if (v <= 1e-9) {
        // Komentar ini bisa diaktifkan untuk debugging jika diperlukan:
        // std::cerr << "Peringatan: Kecepatan v (" << v << ") sangat kecil atau nol dalam f_drone(). Mengembalikan nilai besar." << std::endl;
        return 1e12; // Mengembalikan angka yang sangat besar untuk menandakan masalah atau mendorong solusi menjauh dari daerah ini.
    }
    return 2.0 * c1 * v - (2.0 * c2 / (v * v * v));
}

// Fungsi df_drone(v, c1, c2)
// Menghitung nilai turunan pertama dari f_drone(v) terhadap v, yaitu f'(v) = 2*c1 + 6*c2*v^(-4).
// Parameter:
//   v  (double): Kecepatan drone (m/s).
//   c1 (double): Koefisien terkait gaya hambat parasitik drone.
//   c2 (double): Koefisien terkait gaya hambat terinduksi drone.
// Mengembalikan (double): Nilai f'(v).
double df_drone(double v, double c1, double c2) {
    // Memastikan v positif.
    if (v <= 1e-9) {
        // Komentar ini bisa diaktifkan untuk debugging jika diperlukan:
        // std::cerr << "Peringatan: Kecepatan v (" << v << ") sangat kecil atau nol dalam df_drone(). Mengembalikan nilai besar." << std::endl;
        return 1e12; // Mengembalikan angka yang sangat besar.
    }
    return 2.0 * c1 + (6.0 * c2 / (v * v * v * v));
}

// Struktur TestCaseResult
// Digunakan untuk menyimpan semua hasil dan parameter yang relevan dari satu kasus uji
// metode Newton-Raphson.
struct TestCaseResult {
    double c1_param;             // Koefisien c1 yang digunakan dalam kasus uji.
    double c2_param;             // Koefisien c2 yang digunakan dalam kasus uji.
    double v0_initial;           // Tebakan awal kecepatan (v0) yang digunakan.
    double v_optimal_numerical;  // Kecepatan optimal yang ditemukan secara numerik.
    int iterations_taken;        // Jumlah iterasi yang diperlukan untuk konvergensi atau mencapai batas.
    double v_optimal_analytical; // Kecepatan optimal yang dihitung secara analitik (jika tersedia).
    double relative_error_percent; // Persentase error relatif antara solusi numerik dan analitik.
    std::string status_message;  // Pesan status yang menjelaskan hasil (misalnya, "Converged", "Max Iterations Reached").
};

// Fungsi newton_raphson_solver
// Mengimplementasikan metode Newton-Raphson untuk mencari akar fungsi f_drone(v) = 0,
// yang merepresentasikan kecepatan optimal drone.
// Parameter:
//   c1 (double): Koefisien c1 untuk model daya drone.
//   c2 (double): Koefisien c2 untuk model daya drone.
//   v0 (double): Tebakan awal kecepatan (v0) dalam m/s.
//   tolerance (double): Toleransi error untuk kriteria konvergensi.
//   max_iter (int): Jumlah iterasi maksimum yang diizinkan.
//   print_iterations_detail (bool): Flag untuk mencetak detail setiap iterasi (opsional, default false).
// Mengembalikan (TestCaseResult): Struktur yang berisi hasil dari proses pencarian akar.
TestCaseResult newton_raphson_solver(double c1, double c2, double v0, double tolerance, int max_iter, bool print_iterations_detail = false) {
    double v_current = v0; // Kecepatan saat ini, diinisialisasi dengan tebakan awal v0.
    int iter = 0;          // Penghitung iterasi.
    // Inisialisasi struktur hasil dengan nilai default dan parameter input.
    TestCaseResult result = {c1, c2, v0, v0, 0, 0.0, -1.0, "Max Iterations Reached"}; // Status default jika tidak konvergen.

    // Pemeriksaan input awal yang tidak valid
    if (v0 <= 0) {
        result.status_message = "Invalid Initial Guess (v0 <= 0)"; // Tebakan awal tidak boleh nol atau negatif.
        result.iterations_taken = 0;
        result.v_optimal_numerical = v0; // Mengembalikan v0 sebagai indikasi.
        return result;
    }
     if (c1 <= 0 || c2 <= 0) {
        result.status_message = "Invalid Parameters (c1 or c2 <= 0)"; // Parameter c1 dan c2 harus positif.
        result.iterations_taken = 0;
        result.v_optimal_numerical = v0;
        return result;
    }

    // Mencetak header tabel detail iterasi jika diminta.
    if (print_iterations_detail) {
        std::cout << "\n--- Detail Iterasi untuk c1=" << c1 << ", c2=" << c2 << ", v0=" << v0 << " ---\n";
        std::cout << std::setw(10) << "Iterasi"
                  << std::setw(18) << "v_i (m/s)"
                  << std::setw(18) << "f(v_i)"
                  << std::setw(18) << "f'(v_i)"
                  << std::setw(18) << "v_{i+1} (m/s)"
                  << std::setw(20) << "|v_{i+1}-v_i|" << std::endl;
        std::cout << std::string(102, '-') << std::endl;
    }

    // Loop utama metode Newton-Raphson
    while (iter < max_iter) {
        double f_val = f_drone(v_current, c1, c2);   // Hitung f(v_i)
        double df_val = df_drone(v_current, c1, c2); // Hitung f'(v_i)

        // Penanganan kasus jika turunan (df_val) terlalu dekat dengan nol (potensi pembagian dengan nol).
        if (std::abs(df_val) < 1e-10) {
            if (print_iterations_detail) std::cerr << "Error: Turunan terlalu dekat dengan nol pada v = " << v_current << std::endl;
            result.status_message = "Derivative Zero"; // Turunan nol.
            result.v_optimal_numerical = v_current;
            result.iterations_taken = iter;
            return result;
        }

        // Formula Newton-Raphson: v_{i+1} = v_i - f(v_i) / f'(v_i)
        double v_next = v_current - f_val / df_val;
        // Hitung perubahan absolut antara iterasi saat ini dan berikutnya.
        double change = std::abs(v_next - v_current);

        // Penanganan jika solusi mengarah ke kecepatan non-fisik (negatif atau nol).
        if (v_next <= 0) {
             if (print_iterations_detail) std::cerr << "Error: Kecepatan berikutnya non-positif (v_next = " << v_next << ")" << std::endl;
            result.status_message = "Non-Positive Velocity"; // Kecepatan non-positif.
            result.v_optimal_numerical = v_current; // Simpan kecepatan valid terakhir.
            result.iterations_taken = iter;
            return result;
        }

        // Mencetak detail iterasi saat ini jika diminta.
        if (print_iterations_detail) {
            std::cout << std::fixed << std::setprecision(8) // Atur presisi output.
                      << std::setw(10) << iter
                      << std::setw(18) << v_current
                      << std::setw(18) << f_val
                      << std::setw(18) << df_val
                      << std::setw(18) << v_next
                      << std::setw(20) << change << std::endl;
        }

        // Memeriksa kriteria konvergensi.
        if (change < tolerance) {
            if (print_iterations_detail) std::cout << "\nKonvergen setelah " << iter + 1 << " iterasi!" << std::endl;
            result.status_message = "Converged"; // Konvergen.
            result.v_optimal_numerical = v_next;
            result.iterations_taken = iter + 1;
            return result;
        }

        v_current = v_next; // Perbarui v_current untuk iterasi berikutnya.
        iter++;             // Naikkan penghitung iterasi.
    }

    // Jika loop selesai tanpa konvergensi, berarti iterasi maksimum tercapai.
    if (print_iterations_detail) std::cerr << "Peringatan: Iterasi maksimum (" << max_iter << ") tercapai tanpa konvergensi." << std::endl;
    result.v_optimal_numerical = v_current; // Simpan kecepatan terakhir yang dihitung.
    result.iterations_taken = iter;
    // result.status_message sudah diatur ke "Max Iterations Reached" secara default.
    return result;
}

// Fungsi print_summary_table
// Mencetak tabel ringkasan hasil dari semua kasus uji yang dijalankan.
// Parameter:
//   results (const std::vector<TestCaseResult>&): Vektor berisi hasil dari setiap kasus uji.
void print_summary_table(const std::vector<TestCaseResult>& results) {
    std::cout << "\n\n--- Tabel Ringkasan Hasil Newton-Raphson untuk Optimasi Drone ---" << std::endl;
    // Mencetak header tabel.
    std::cout << std::setw(10) << "c1"
              << std::setw(10) << "c2"
              << std::setw(15) << "v0_awal"
              << std::setw(20) << "v_opt_numerik"
              << std::setw(12) << "Iterasi"
              << std::setw(20) << "v_opt_analitik"
              << std::setw(18) << "Err.Rel.(%)"
              << std::setw(25) << "Status" << std::endl;
    std::cout << std::string(130, '-') << std::endl; // Garis pemisah.

    // Melakukan iterasi melalui setiap hasil dalam vektor dan mencetaknya.
    for (const auto& res : results) {
        std::cout << std::fixed << std::setprecision(5) // Atur presisi output.
                  << std::setw(10) << res.c1_param
                  << std::setw(10) << res.c2_param
                  << std::setw(15) << res.v0_initial
                  << std::setw(20) << res.v_optimal_numerical
                  << std::setw(12) << res.iterations_taken
                  << std::setw(20) << res.v_optimal_analytical;
        // Menangani pencetakan error relatif (N/A jika tidak berlaku).
        if (res.relative_error_percent >= 0) {
            std::cout << std::setw(18) << std::setprecision(5) << res.relative_error_percent;
        } else {
            std::cout << std::setw(18) << "N/A";
        }
        std::cout << std::setw(25) << res.status_message << std::endl;
    }
    std::cout << std::string(130, '-') << std::endl; // Garis pemisah.
}

// Fungsi utama program
int main() {
    std::vector<TestCaseResult> all_results; // Vektor untuk menyimpan semua hasil kasus uji.
    std::string input_filename = "synthetic_data.txt"; // Nama file input.
    std::ifstream infile(input_filename); // Membuat objek stream file input.

    // Memeriksa apakah file input berhasil dibuka.
    if (!infile.is_open()) {
        std::cerr << "Error: Tidak dapat membuka file input: " << input_filename << std::endl;
        std::cerr << "Pastikan file 'synthetic_data.txt' ada di direktori yang sama atau jalankan generator data (Data.cpp) terlebih dahulu." << std::endl;
        return 1; // Mengembalikan kode error.
    }

    std::cout << "Membaca data dari " << input_filename << " untuk Optimasi Kecepatan Drone...\n" << std::endl;

    std::string line;     // Untuk menyimpan setiap baris dari file.
    int line_number = 0;  // Penghitung baris untuk pelaporan error.
    // Flag untuk mengontrol pencetakan detail iterasi (bisa diaktifkan untuk debugging).
    // bool show_detailed_iterations_for_first_case = true; 

    // Membaca file baris per baris.
    while (std::getline(infile, line)) {
        line_number++;
        std::istringstream iss(line); // Membuat stream string untuk parsing baris.
        // Variabel untuk menyimpan parameter yang dibaca dari baris.
        double c1_in, c2_in, v0_in, tol_in;
        int max_iter_in;

        // Mencoba mem-parsing parameter dari baris.
        // Format yang diharapkan: c1 c2 v0_awal epsilon iterasi_maks
        if (!(iss >> c1_in >> c2_in >> v0_in >> tol_in >> max_iter_in)) {
            std::cerr << "Error parsing baris ke-" << line_number << ": " << line << std::endl;
            std::cerr << "Format yang diharapkan: c1 c2 v0_awal epsilon iterasi_maks" << std::endl;
            continue; // Lewati baris ini dan coba baris berikutnya.
        }

        // Opsi untuk mencetak detail iterasi untuk kasus tertentu (misalnya, kasus pertama).
        // bool print_details_this_case = (line_number == 1 && show_detailed_iterations_for_first_case);
        bool print_details_this_case = false; // Atur ke true untuk melihat detail iterasi untuk kasus ini.

        // Memanggil solver Newton-Raphson dengan parameter yang dibaca.
        TestCaseResult current_res = newton_raphson_solver(c1_in, c2_in, v0_in, tol_in, max_iter_in, print_details_this_case);

        // Menghitung solusi analitik dan error relatif.
        if (c1_in > 0 && c2_in > 0) { // Solusi analitik valid jika c1 dan c2 positif.
            current_res.v_optimal_analytical = std::pow(c2_in / c1_in, 0.25);
            // Hitung error relatif hanya jika konvergen dan solusi analitik valid.
            if (current_res.status_message == "Converged" && current_res.v_optimal_analytical > 0) {
                current_res.relative_error_percent = std::abs(current_res.v_optimal_numerical - current_res.v_optimal_analytical) / current_res.v_optimal_analytical * 100.0;
            } else {
                current_res.relative_error_percent = -1.0; // Menandakan N/A atau error.
            }
        } else {
            current_res.v_optimal_analytical = -1.0; // N/A jika c1 atau c2 tidak positif.
            current_res.relative_error_percent = -1.0;
        }
        all_results.push_back(current_res); // Tambahkan hasil kasus uji ke vektor.
    }

    infile.close(); // Menutup file input.

    // Memeriksa apakah ada hasil yang diproses sebelum mencetak tabel.
    if (all_results.empty() && line_number == 0) {
        std::cout << "File " << input_filename << " kosong atau tidak ada data yang valid." << std::endl;
    } else if (all_results.empty() && line_number > 0) {
        std::cout << "Tidak ada data yang berhasil diproses dari " << input_filename << "." << std::endl;
    }
    else {
        print_summary_table(all_results); // Mencetak tabel ringkasan jika ada hasil.
    }

    return 0; // Mengindikasikan program berakhir dengan sukses.
}
