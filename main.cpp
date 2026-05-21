#include <iostream>
#include <string>
#include <cstdlib> // buat clear screen

using namespace std;

struct Menu {
    int id;
    string nama;
    int harga;
    string kategori;
    Menu* left;
    Menu* right;
};

struct CartNode {
    int idMenu;
    string namaMenu;
    int harga;
    int qty;
    int subtotal;
    CartNode* prev;
    CartNode* next;
};

struct Order {
    int orderId;
    string namaPemesan;
    int status; 
};

Menu* rootMenu = NULL;
CartNode* headCart = NULL;
CartNode* tailCart = NULL;

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

Menu* createMenuNode(int id, string nama, int harga, string kategori) {
    Menu* newNode = new Menu();
    newNode->id = id;
    newNode->nama = nama;
    newNode->harga = harga;
    newNode->kategori = kategori;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Menu* insertMenu(Menu* root, int id, string nama, int harga, string kategori) {
    if (root == NULL) {
        return createMenuNode(id, nama, harga, kategori);
    }
    // Jika ID lebih kecil, masuk ke kiri. Jika lebih besar, ke kanan.
    if (id < root->id) {
        root->left = insertMenu(root->left, id, nama, harga, kategori);
    } else if (id > root->id) {
        root->right = insertMenu(root->right, id, nama, harga, kategori);
    } else {
        cout << "Gagal: Menu dengan ID " << id << " sudah ada!\n";
    }
    return root;
}

void tampilKatalogInOrder(Menu* root) {
    if (root != NULL) {
        tampilKatalogInOrder(root->left);
        cout << "| " << root->id << "\t| Rp" << root->harga << "\t| " << root->kategori << "\t| " << root->nama << endl;
        tampilKatalogInOrder(root->right);
    }
}

Menu* searchMenu(Menu* root, int id) {
    if (root == NULL || root->id == id) {
        return root;
    }
    if (root->id < id) {
        return searchMenu(root->right, id);
    }
    return searchMenu(root->left, id);
}

void tampilKeranjang() {
    if (headCart == NULL) {
        cout << "\nKeranjang belanja Anda masih kosong.\n";
        return;
    }
    
    CartNode* temp = headCart;
    int totalBelanja = 0;
    
    cout << "\n--------------------------------------------------------\n";
    cout << "ID\t| Qty\t| Subtotal\t| Nama Menu\n";
    cout << "--------------------------------------------------------\n";
    while (temp != NULL) {
        cout << temp->idMenu << "\t| " << temp->qty << "\t| Rp" << temp->subtotal << "\t| " << temp->namaMenu << "\n";
        totalBelanja += temp->subtotal;
        temp = temp->next;
    }
    cout << "--------------------------------------------------------\n";
    cout << "TOTAL PEMBAYARAN: Rp" << totalBelanja << "\n";
}

void layarPemesan() {
    int pilihan;
    while (true) {
        printHeader("Dashboard Pemesan");
        cout << "1. Lihat Katalog Menu (Tree - Kungs)\n";
        cout << "2. Cari Menu berdasarkan ID (Tree)\n";
        cout << "3. Kelola Keranjang & Checkout (DLL & Stack - Akbats)\n";
        cout << "4. Lacak Pesanan Selesai (SLL - Danis Pixel World)\n";
        cout << "0. Kembali ke Login\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            printHeader("Katalog Menu");
            cout << "| ID\t| Harga\t| Katergori\t| Nama Menu\n";
            cout << "--------------------------------------------------------\n";
            if (rootMenu == NULL) {
                cout << "Katalog masih kosong. Resto belum menambahkan menu.\n";
            } else {
                tampilKatalogInOrder(rootMenu);
            }
            cout << "Tekan Enter untuk kembali...";
            cin.ignore(); cin.get();
        }
        else if (pilihan == 2) {
            int cariId;
            cout << "Masukkan ID Menu yang dicari: ";
            cin >> cariId;
            Menu* hasilCari = searchMenu(rootMenu, cariId);

            if (hasilCari != NULL) {
                cout << "Menu ditemukan:\n";
                cout << "Nama: " << hasilCari->nama << "\n";
                cout << "Kategori: " << hasilCari->kategori << "\n";
                cout << "Harga: Rp" << hasilCari->harga << "\n";
            } else {
                cout << "Menu dengan ID " << cariId << " tidak ditemukan.\n";
            }
            cout << "Tekan Enter untuk kembali...";
            cin.ignore(); cin.get();
        }
        else if (pilihan == 3) {
            int pilCart;
            while (true) {
                printHeader("Kelola Keranjang Belanja");
                cout << "1. Tambah Menu ke Keranjang\n";
                cout << "2. Lihat Isi Keranjang\n";
                cout << "3. Undo (Batalkan Item Terakhir)\n";
                cout << "4. Checkout & Bayar\n";
                cout << "0. Kembali ke Menu Pemesan\n";
                cout << "Pilih: ";
                cin >> pilCart;

                if (pilCart == 1) {
                    int cariId, qty;
                    cout << "\nMasukkan ID Menu: "; cin >> cariId;
                    Menu* menu = searchMenu(rootMenu, cariId);
                    
                    if (menu != NULL) {
                        cout << "Ditemukan: " << menu->nama << " (Rp" << menu->harga << ")\n";
                        cout << "Masukkan Jumlah (Qty): "; cin >> qty;
                    } else {
                        cout << "Gagal: Menu ID " << cariId << " tidak ada di katalog.\n";
                    }
                    cout << "Tekan Enter untuk lanjut...";
                    cin.ignore(); cin.get();
                } 
                else if (pilCart == 2) {
                    printHeader("Isi Keranjang Belanja");
                    tampilKeranjang();
                    cout << "\nTekan Enter untuk lanjut...";
                    cin.ignore(); cin.get();
                } 
                else if (pilCart == 3) {
                    cout << "\nTekan Enter untuk lanjut...";
                    cin.ignore(); cin.get();
                }
                else if (pilCart == 4) {
                    printHeader("Checkout Pembayaran");
                    cout << "\nTekan Enter untuk lanjut...";
                    cin.ignore(); cin.get();
                }
                else if (pilCart == 0) {
                    break;
                }
            }
        }
        else if (pilihan == 0) {
            break;
        }
        else {
            cout << "Input tidak valid.\n";
            cin.ignore(); cin.get();
        }
    }
}

void layarResto() {
    int pilihan;
    while (true) {
        printHeader("Dashboard Admin Resto");
        cout << "1. Tambah Menu Baru (Tree - Kungs)\n";
        cout << "2. Monitor Antrean Dapur (Queue - Danis Pixel World)\n";
        cout << "3. Laporan Pendapatan (SLL - Danis Pixel World)\n";
        cout << "0. Kembali ke Login\n";
        cout << "Pilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            int id, harga;
            string nama, kategori;

            printHeader("Tambah Menu Baru");
            cout << "Masukkan ID Menu: "; cin >> id;
            cout << "Masukkan Nama Menu: "; cin.ignore(); getline(cin, nama);
            cout << "Masukkan Kategori Menu: "; getline(cin, kategori);
            cout << "Masukkan Harga Menu: "; cin >> harga;

            rootMenu = insertMenu(rootMenu, id, nama, harga, kategori);
            cout << "\n[SUKSES] Menu " << nama << " berhasil ditambahkan!\n";

            cout << "Tekan Enter untuk kembali...";
            cin.ignore(); cin.get();
        }
        else if (pilihan == 0) {
            break;
        }
    }
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