#include <iostream>
#include <string>
#include <cstdlib> // buat clear screen

using namespace std;

struct Menu {
    int id;
    string nama;
    int harga;
    string kategori;
};

struct Order {
    int orderId;
    string namaPemesan;
    int status; 
};

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printHeader(string title) {
    clearScreen();
    cout << "+--------------------------------------------------------+\n";
    cout << "|                  FOOD DELIVERY SYSTEM                  |\n";
    cout << "+--------------------------------------------------------+\n";
    
    int spaces = 54 - title.length();
    cout << "| " << title;
    for(int i = 0; i < spaces; i++) {
        cout << " ";
    }
    cout << " |\n";
    cout << "+--------------------------------------------------------+\n";
}

void layarPemesan() {
    printHeader("Dashboard Pemesan");
    cout << "1. Lihat Katalog Menu (Tree - Kungs)\n";
    cout << "2. Kelola Keranjang & Checkout (DLL & Stack - Akbats)\n";
    cout << "3. Lacak Pesanan Selesai (SLL - Danis Pixel World)\n";
    cout << "0. Kembali ke Login\n";
    cout << "Pilih: ";
}

void layarResto() {
    printHeader("Dashboard Admin Resto");
    cout << "1. Tambah Menu Baru (Tree - Kungs)\n";
    cout << "2. Monitor Antrean Dapur (Queue - Danis Pixel World)\n";
    cout << "3. Laporan Pendapatan (SLL - Danis Pixel World)\n";
    cout << "0. Kembali ke Login\n";
    cout << "Pilih: ";
}

void layarKurir() {
    printHeader("Dashboard Kurir");
    cout << "1. Radar Orderan Siap Antar (Weighted Graph - Patah)\n";
    cout << "2. Navigasi Rute Terpendek (Dijkstra/BFS - Patah)\n";
    cout << "0. Kembali ke Login\n";
    cout << "Pilih: ";
}

void jalankanSistem() {
    int pilihanRole;
    bool aplikasiBerjalan = true;

    while (aplikasiBerjalan) {
        printHeader("Gateway Login");
        cout << "Masuk Sebagai:\n";
        cout << "1. Pemesan\n";
        cout << "2. Admin Resto\n";
        cout << "3. Kurir\n";
        cout << "0. Tutup Aplikasi\n";
        cout << "Pilih: ";
        
        if (!(cin >> pilihanRole)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (pilihanRole) {
            case 1:
                layarPemesan();
                cin.ignore(); cin.get(); 
                break;
            case 2:
                layarResto();
                cin.ignore(); cin.get();
                break;
            case 3:
                layarKurir();
                cin.ignore(); cin.get();
                break;
            case 0:
                aplikasiBerjalan = false;
                break;
            default:
                cout << "Input tidak valid.\n";
                cin.ignore(); cin.get();
        }
    }
}

int main() {
    jalankanSistem();
    return 0;
}