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

struct AntreDapur {
    int id;
    string daftarPesanan;
    int totalHarga;
    int status;
};

struct HistoryNode {
    AntreDapur data;
    HistoryNode* next;
};

Menu* rootMenu = NULL;
CartNode* headCart = NULL;
CartNode* tailCart = NULL;

AntreDapur antrean[100];
AntreDapur pesananSelesai[100];

int jumlahPesananSelesai = 0;
int frontAntrean = -1;
int rearAntrean = -1;
int idAntrean = 1;

HistoryNode* headHistory = NULL;

string statusPesanan(int status) {
    if (status == 1) return "Sedang dibuat";
    if (status == 2) return "Menunggu Kurir";
    if (status == 3) return "Sedang dikirim kurir";
    if (status == 4) return "Selesai";
    return "Unknown";
}

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

void pushCart(int id, string nama, int harga, int qty) {
    CartNode* newNode = new CartNode();
    newNode->idMenu = id;
    newNode->namaMenu = nama;
    newNode->harga = harga;
    newNode->qty = qty;
    newNode->subtotal = harga * qty;
    newNode->prev = NULL;
    newNode->next = NULL;

    if (headCart == NULL) {
        headCart = tailCart = newNode;
    } else {
        tailCart->next = newNode;
        newNode->prev = tailCart;
        tailCart = newNode; 
    }
    cout << "\n[BERHASIL] " << qty << "x " << nama << " masuk ke keranjang!\n";
}

void popCart() {
    if (tailCart == NULL) {
        cout << "\n[INFO] Keranjang kosong. Tidak ada yang bisa di-undo.\n";
        return;
    }
    
    CartNode* temp = tailCart;
    cout << "\n[UNDO SUKSES] Membatalkan pesanan terakhir: " << temp->namaMenu << "\n";
    
    if (headCart == tailCart) {
        headCart = tailCart = NULL;
    } else {
        tailCart = tailCart->prev;
        tailCart->next = NULL;
    }
    delete temp; 
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

void enqueueDapur(string daftarPesanan, int totalHarga) {

    if (rearAntrean == 99) {
        cout << "\nAntrean dapur penuh!\n";
        return;
    }

    if (frontAntrean == -1) {
        frontAntrean = 0;
    }

    rearAntrean++;

    antrean[rearAntrean].id = idAntrean++;
    antrean[rearAntrean].daftarPesanan = daftarPesanan;
    antrean[rearAntrean].totalHarga = totalHarga;
    antrean[rearAntrean].status = 1; // Sedang dibuat

    cout << "\n[QUEUE] Pesanan masuk ke antrean dapur!\n";
}

void checkoutCart() {
    if (headCart == NULL) {
        cout << "\n[INFO] Keranjang masih kosong. Pilih menu dulu ya!\n";
        return;
    }
    
    tampilKeranjang();
    cout << "\nSedang memproses pembayaran...\n";
    cout << "[SUKSES] Pembayaran Berhasil! Pesanan diteruskan ke Dapur Resto.\n";
    
    string daftarPesanan = "";
    int totalBelanja = 0;

    CartNode* tempData = headCart;

    while (tempData != NULL) {

        daftarPesanan += to_string(tempData->qty)
                    + "x "
                    + tempData->namaMenu;

        if (tempData->next != NULL) {
            daftarPesanan += ", ";
        }

        totalBelanja += tempData->subtotal;

        tempData = tempData->next;
    }

    // kirim ke antrean dapur
    enqueueDapur(daftarPesanan, totalBelanja);
    while (headCart != NULL) {
        CartNode* temp = headCart;
        headCart = headCart->next;
        delete temp;
    }
    tailCart = NULL;
}

void tambahHistory(AntreDapur dataPesanan) {

    HistoryNode* newNode = new HistoryNode();

    newNode->data = dataPesanan;
    newNode->next = NULL;

    if (headHistory == NULL) {
        headHistory = newNode;
    }
    else {

        HistoryNode* temp = headHistory;

        while (temp->next != NULL) {
            temp = temp->next;
        }

        temp->next = newNode;
    }
}

void tampilAntreanDapur() {

    if (frontAntrean == -1) {

        cout << "\nTidak ada antrean dapur.\n";
        return;
    }

    cout << "\n================ ANTREAN DAPUR ================\n\n";

    for (int i = frontAntrean; i <= rearAntrean; i++) {

        cout << "[-->] Antrean ID ";

        if (antrean[i].id < 10)
            cout << "00" << antrean[i].id << endl;
        else if (antrean[i].id < 100)
            cout << "0" << antrean[i].id << endl;
        else
            cout << antrean[i].id << endl;

        cout << "      Pesanan : "
             << antrean[i].daftarPesanan << endl;

        cout << "      Total   : Rp"
             << antrean[i].totalHarga << endl;

        cout << endl;
    }
}

void selesaikanPesanan() {

    if (frontAntrean == -1) {

        cout << "\nTidak ada antrean untuk diproses.\n";
        return;
    }

    AntreDapur selesai = antrean[frontAntrean];
    selesai.status = 2; // Menunggu kurir
    

    cout << "\n[SELESAI]\n";
    cout << "Pesanan ID ";

    if (selesai.id < 10)
        cout << "00" << selesai.id;
    else if (selesai.id < 100)
        cout << "0" << selesai.id;
    else
        cout << selesai.id;

    cout << " selesai dimasak.\n";


    pesananSelesai[jumlahPesananSelesai] = selesai;
    jumlahPesananSelesai++;

    // masuk history
    tambahHistory(selesai);

    // dequeue
    if (frontAntrean == rearAntrean) {

        frontAntrean = rearAntrean = -1;
    }
    else {

        frontAntrean++;
    }
}

void tampilHistoryDapur() {

    if (headHistory == NULL) {

        cout << "\nHistory dapur masih kosong.\n";
        return;
    }

    HistoryNode* temp = headHistory;

    int totalPendapatan = 0;

    cout << "\n================ HISTORY DAPUR ================\n\n";

    while (temp != NULL) {

        cout << "[-->] ID ";

        if (temp->data.id < 10)
            cout << "00" << temp->data.id << endl;
        else if (temp->data.id < 100)
            cout << "0" << temp->data.id << endl;
        else
            cout << temp->data.id << endl;

        cout << "      Pesanan : "
             << temp->data.daftarPesanan << endl;

        cout << "      Total   : Rp"
             << temp->data.totalHarga << endl;

        cout << endl;

        totalPendapatan += temp->data.totalHarga;

        temp = temp->next;
    }

    cout << "TOTAL PENDAPATAN : Rp"
         << totalPendapatan << endl;
}

void lacakPesanan() {
    bool adaPesanan = false;
    if (frontAntrean != -1) {
        for (int i = frontAntrean; i <= rearAntrean; i++) {
            cout << "[-->] Pesanan ID ";
            if (antrean[i].id < 10) cout << "00" << antrean[i].id;
            else if (antrean[i].id < 100) cout << "0" << antrean[i].id;
            else cout << antrean[i].id;
            cout << endl;
            cout << "      Pesanan : " << antrean[i].daftarPesanan << endl;
            cout << "      Status  : " << statusPesanan(antrean[i].status) << endl;
            cout << "      Total   : Rp" << antrean[i].totalHarga << endl;
            cout << endl;
            adaPesanan = true;
        }
    }

    for (int i = 0; i < jumlahPesananSelesai; i++) {
        cout << "[-->] Pesanan ID ";
        if (pesananSelesai[i].id < 10) cout << "00" << pesananSelesai[i].id;
        else if (pesananSelesai[i].id < 100) cout << "0" << pesananSelesai[i].id;
        else cout << pesananSelesai[i].id;
        cout << endl;
        cout << "      Pesanan : " << pesananSelesai[i].daftarPesanan << endl;
        cout << "      Status  : " << statusPesanan(pesananSelesai[i].status) << endl;
        cout << "      Total   : Rp" << pesananSelesai[i].totalHarga << endl;
        cout << endl;
        adaPesanan = true;
    }

    if (!adaPesanan) {
        cout << "\nBelum ada pesanan.\n";
    }
}

void layarPemesan() {
    int pilihan;
    while (true) {
        printHeader("Dashboard Pemesan");
        cout << "1. Lihat Katalog Menu (Tree - Kungs)\n";
        cout << "2. Cari Menu berdasarkan ID (Tree)\n";
        cout << "3. Kelola Keranjang & Checkout (DLL & Stack - Akbats)\n";
        cout << "4. Lacak Status Pesanan\n";
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
                        pushCart(menu->id, menu->nama, menu->harga, qty);
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
                    popCart();
                    cout << "\nTekan Enter untuk lanjut...";
                    cin.ignore(); cin.get();
                }
                else if (pilCart == 4) {
                    printHeader("Checkout Pembayaran");
                    checkoutCart();
                    cout << "\nTekan Enter untuk lanjut...";
                    cin.ignore(); cin.get();
                }
                else if (pilCart == 0) {
                    break;
                }
            }
        }
        else if (pilihan == 4){
            printHeader("Lacak Pesanan");

            lacakPesanan();

            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
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
        cout << "2. Monitor Antrean Dapur\n";
        cout << "3. History dapur & Laporan Pendapatan\n";
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

        else if (pilihan == 2) {

            int pilihAntrean;

            while (true) {

                printHeader("Monitor Antrean Dapur");
                tampilAntreanDapur();

                cout << "1. Selesaikan Pesanan Paling Depan\n";
                cout << "0. Kembali ke Menu Admin\n";
                cout << "Pilih: ";
                cin >> pilihAntrean;

                if (pilihAntrean == 1) {

                    selesaikanPesanan();

                    cout << "\nTekan Enter untuk lanjut...";
                    cin.ignore();
                    cin.get();
                }
                else if (pilihAntrean == 0) {
                    break;
                }
            }
        }

        else if (pilihan == 3) {

            printHeader("History Dapur");

            tampilHistoryDapur();

            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
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