#include <iostream>
#include <string>
#include <cstdlib> // buat clear screen
#include <limits.h> // buat batas maksimal int

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

struct AntreDapur {
    int id;
    string daftarPesanan;
    int totalHarga;
    int status;
    int idTujuan;
};

struct HistoryNode {
    AntreDapur data;
    HistoryNode* next;
};

Menu* rootMenu = NULL;
CartNode* headCart = NULL;
CartNode* tailCart = NULL;

Menu daftarMenu[10] = {
    {1, "Americano", 15000, "Minuman"},
    {2, "Cappuccino", 18000, "Minuman"},
    {3, "Latte", 20000, "Minuman"},
    {4, "Matcha", 22000, "Minuman"},
    {5, "Chocolate", 21000, "Minuman"},
    {6, "Croissant", 12000, "Makanan"},
    {7, "Donat", 10000, "Makanan"},
    {8, "Sandwich", 17000, "Makanan"},
    {9, "French Fries", 15000, "Makanan"},
    {10, "Cheesecake", 25000, "Makanan"}
};


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

void tampilkanMenu(Menu menu[], int size) {
    cout << "\n===== MENU CAFE =====" << endl;
    for(int i = 0; i < size; i++) {
        cout << menu[i].id << ". "
             << menu[i].nama << " - "
             << menu[i].kategori << " - Rp"
             << menu[i].harga << endl;
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

void enqueueDapur(string daftarPesanan, int totalHarga, int tujuan) {

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
    antrean[rearAntrean].idTujuan = tujuan;

    cout << "\n[QUEUE] Pesanan masuk ke antrean dapur!\n";
}

void checkoutCart() {
    if (headCart == NULL) {
        cout << "\n[INFO] Keranjang masih kosong. Pilih menu dulu ya!\n";
        return;
    }

    clearScreen();
    cout << "+--------------------------------------------------------+\n";
    cout << "|                  CHECKOUT PEMBAYARAN                   |\n";
    cout << "+--------------------------------------------------------+\n";
    
    tampilKeranjang();
    
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

    int tujuanKirim;
    cout << "\nPilih Lokasi Pengiriman:\n";
    cout << "1. Cileunyi\n2. Ujungberung\n3. Antapani\n4. Cibiru\n";
    cout << "Tujuan Anda: ";
    cin >> tujuanKirim;

    char konfirmasi;
    cout << "\nTotal tagihan Anda Rp" << totalBelanja << ".\n";
    cout << "Apakah Anda yakin ingin membayar? (Y/N): ";
    cin >> konfirmasi;

    if (konfirmasi == 'Y' || konfirmasi == 'y') {
        cout << "\nSedang memproses pembayaran...\n";
        
        // kirim ke antrean dapur
        enqueueDapur(daftarPesanan, totalBelanja, tujuanKirim);
        
        while (headCart != NULL) {
            CartNode* temp = headCart;
            headCart = headCart->next;
            delete temp;
        }
        tailCart = NULL;
        cout << "[SUKSES] Pembayaran Berhasil! Pesanan diteruskan ke Dapur Resto.\n";
    } else {
        cout << "\n[INFO] Checkout dibatalkan. Pesanan masih tersimpan di keranjang.\n";
    }
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

    if (jumlahPesananSelesai >= 100) {
    cout << "\n[Penuh] Kapasitas memori riwayat sudah penuh!\n";
    return;
}

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
        printHeader("DASHBOARD PEMESAN");
        cout << "1. Lihat Katalog Menu\n";
        cout << "2. Cari Menu berdasarkan ID\n";
        cout << "3. Kelola Keranjang & Checkout\n";
        cout << "4. Lacak Status Pesanan\n";
        cout << "0. Kembali ke Login\n";
        cout << "Pilih: ";

        if (!(cin >> pilihan)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Input harus angka!\n";
                cin.get();
                continue;
            }

        if (pilihan == 1) {
            printHeader("Katalog Menu");
            cout << "| ID\t| Harga\t| Kategori\t| Nama Menu\n";
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
                    printHeader("Tambah Menu ke Keranjang");
                    cout << "| ID\t| Harga\t| Kategori\t| Nama Menu\n";
                    cout << "--------------------------------------------------------\n";
                    if (rootMenu == NULL) {
                        cout << "Katalog masih kosong.\n";
                    } else {
                        tampilKatalogInOrder(rootMenu);
                    }
                    cout << "--------------------------------------------------------\n";
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
        printHeader("DASHBOARD ADMIN RESTO");
        cout << "1. Tambah Menu Baru\n";
        cout << "2. Monitor Antrean Dapur\n";
        cout << "3. History Dapur & Laporan Pendapatan\n";
        cout << "0. Kembali ke Login\n";
        cout << "Pilih: ";

        if (!(cin >> pilihan)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus angka!\n";
            cin.get();
            continue;
        }

        if (pilihan == 1) {
            int id, harga;
            string nama, kategori;
            char konfirmasi;

            printHeader("TAMBAH MENU BARU");
            cout << "Ketik '0' pada ID untuk membatalkan.\n\n";
            
            cout << "Masukkan ID Menu: "; cin >> id;
            if (id == 0) {
                cout << "\n[INFO] Penambahan menu dibatalkan.\n";
                cout << "Tekan Enter untuk kembali...";
                cin.ignore(); cin.get();
                continue; 
            }
            
            cout << "Masukkan Nama Menu: "; 
            cin.ignore(10000, '\n');
            getline(cin, nama);
            cout << "Masukkan Kategori Menu: "; getline(cin, kategori);
            cout << "Masukkan Harga Menu: "; cin >> harga;

            cout << "\nSimpan menu ini ke dalam Katalog? (Y/N): ";
            cin >> konfirmasi;

            if (konfirmasi == 'Y' || konfirmasi == 'y') {
                rootMenu = insertMenu(rootMenu, id, nama, harga, kategori);
                cout << "\n[SUKSES] Menu " << nama << " berhasil ditambahkan!\n";
            } else {
                cout << "\n[INFO] Penambahan menu dibatalkan.\n";
            }

            cout << "Tekan Enter untuk kembali...";
            cin.ignore(); cin.get();
        }
        else if (pilihan == 2) {
            int pilihAntrean;
            while (true) {
                printHeader("MONITOR ANTREAN DAPUR");
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
            printHeader("HISTORY DAPUR");
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

bool adaPesananMenungguKurir() {

    for (int i = 0; i < jumlahPesananSelesai; i++) {

        if (pesananSelesai[i].status == 2) {
            return true;
        }
    }

    return false;
}

bool adaPesananDikirim() {

    for (int i = 0; i < jumlahPesananSelesai; i++) {

        if (pesananSelesai[i].status == 3) {
            return true;
        }
    }

    return false;
}

void tampilRadarOrderan() {
    if (jumlahPesananSelesai == 0) {
        cout << "\nBelum ada pesanan siap antar.\n";
        return;
    }

    cout << "\n========== RADAR ORDERAN ==========\n\n";

    bool ada = false;

    for (int i = 0; i < jumlahPesananSelesai; i++) {
        if (pesananSelesai[i].status == 2 || pesananSelesai[i].status == 3) {
            ada = true;

            cout << "Order ID : ";
            if (pesananSelesai[i].id < 10) cout << "00" << pesananSelesai[i].id;
            else if (pesananSelesai[i].id < 100) cout << "0" << pesananSelesai[i].id;
            else cout << pesananSelesai[i].id;
            cout << endl;

            cout << "Pesanan  : " << pesananSelesai[i].daftarPesanan << endl;
            cout << "Status   : " << statusPesanan(pesananSelesai[i].status) << endl;
            cout << "Total    : Rp" << pesananSelesai[i].totalHarga << endl;

            string namaLokasi = "Unknown";
            if (pesananSelesai[i].idTujuan == 1) namaLokasi = "Cileunyi";
            else if (pesananSelesai[i].idTujuan == 2) namaLokasi = "Ujungberung";
            else if (pesananSelesai[i].idTujuan == 3) namaLokasi = "Antapani";
            else if (pesananSelesai[i].idTujuan == 4) namaLokasi = "Cibiru";

            cout << "Tujuan   : " << namaLokasi << endl;
            cout << "-----------------------------------\n";
        }
    }

    if (!ada) {
        cout << "\nTidak ada orderan aktif.\n";
    }
}

void kirimPesanan(){
     int idCari;
    bool ditemukan = false;

    cout << "\nMasukkan ID pesanan yang akan dikirim: ";
    cin >> idCari;

    for (int i = 0; i < jumlahPesananSelesai; i++) {
        if (pesananSelesai[i].id == idCari) {
            if (pesananSelesai[i].status == 2) {
                pesananSelesai[i].status = 3;

                cout << "\n[INFO] Pesanan ID ";

                if (idCari < 10)
                    cout << "00" << idCari;
                else if (idCari < 100)
                    cout << "0" << idCari;
                else
                    cout << idCari;

                cout << " sedang dikirim kurir.\n";
            }

            else {
                cout << "\nPesanan tidak bisa dikirim.\n";
            }

            ditemukan = true;
        }
    }

    if (!ditemukan) {
        cout << "\nID pesanan tidak ditemukan.\n";
    }
}

void selesaikanPengiriman(){
    int idCari;
    bool ditemukan = false;

    cout << "\nMasukkan ID pesanan yang selesai diantar: ";
    cin >> idCari;

    for (int i = 0; i < jumlahPesananSelesai; i++) {

        if (pesananSelesai[i].id == idCari) {
            if (pesananSelesai[i].status == 3) {
                pesananSelesai[i].status = 4;

                cout << "\n[SUKSES] Pesanan ID ";

                if (idCari < 10)
                    cout << "00" << idCari;
                else if (idCari < 100)
                    cout << "0" << idCari;
                else
                    cout << idCari;

                cout << " telah selesai diantar.\n";
            }

            else {
                cout << "\nPesanan belum dikirim kurir.\n";
            }

            ditemukan = true;
        }
    }

    if (!ditemukan) {
        cout << "\nID pesanan tidak ditemukan.\n";
    }
}

void tampilRute(int parent[], int node, string lokasi[]) {

    if (parent[node] == -1) {
        cout << lokasi[node];
        return;
    }

    tampilRute(parent, parent[node], lokasi);

    cout << " -> " << lokasi[node];
}

void navigasiDijkstra() {
    const int JUMLAH_NODE = 5;
    string lokasi[JUMLAH_NODE] = {
        "Resto",
        "Cileunyi",
        "Ujungberung",
        "Antapani",
        "Cibiru"
    };

    int graph[JUMLAH_NODE][JUMLAH_NODE] = {
      //ResCilUjgAntCib
        {0, 4, 6, 0, 0}, //Resto
        {4, 0, 2, 5, 3}, //Cileunyi
        {6, 2, 0, 1, 4}, //Ujungberung
        {0, 5, 1, 0, 2}, //Antapani
        {0, 3, 4, 2, 0}  //Cibiru
    };

    int tujuan;
    cout << "\n========== NAVIGASI TUJUAN ==========\n";
    cout << "1. Cileunyi\n";
    cout << "2. Ujungberung\n";
    cout << "3. Antapani\n";
    cout << "4. Cibiru\n";

    cout << "\nPilih tujuan: ";
    cin >> tujuan;

    if (tujuan < 1 || tujuan > 4) {
        cout << "\nTujuan tidak valid.\n";
        return;
    }

    int start = 0;
    int jarak[JUMLAH_NODE];
    bool visited[JUMLAH_NODE];
    int parent[JUMLAH_NODE];

    for (int i = 0; i < JUMLAH_NODE; i++) {
        jarak[i] = INT_MAX;
        visited[i] = false;
        parent[i] = -1;
    }

    jarak[start] = 0;

    for (int count = 0; count < JUMLAH_NODE - 1; count++) {
        int min = INT_MAX;
        int u = 0;

        for (int v = 0; v < JUMLAH_NODE; v++) {
            if (!visited[v] && jarak[v] <= min) {
                min = jarak[v];
                u = v;
            }
        }

        visited[u] = true;

        for (int v = 0; v < JUMLAH_NODE; v++) {
            if (!visited[v] && graph[u][v] && jarak[u] != INT_MAX && jarak[u] + graph[u][v] < jarak[v]) {
                jarak[v] = jarak[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    cout << "\n===== HASIL NAVIGASI =====\n";
    cout << "Lokasi Awal : Resto\n";
    cout << "Tujuan      : " << lokasi[tujuan] << endl;
    cout << "Rute        : ";
    tampilRute(parent, tujuan, lokasi);
    cout << "\n\nJarak Tempuh: " << jarak[tujuan] << " km\n";
}
void layarKurir() {         

    int pilihan;

    while (true) {

        printHeader("Dashboard Kurir");

        cout << "1. Radar Orderan Siap Antar\n";
        cout << "2. Kirim Pesanan\n";
        cout << "3. Selesaikan Pengiriman\n";
        cout << "4. Navigasi Rute Terpendek\n";
        cout << "0. Kembali ke Login\n";
        cout << "Pilih: ";

        if (!(cin >> pilihan)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus angka!\n";
            cin.get();
            continue;
        }

        if (pilihan == 1) {

            printHeader("Radar Orderan");
            tampilRadarOrderan();

            cout << "\nTekan Enter untuk kembali...";
            cin.ignore();
            cin.get();
        }

        else if (pilihan == 2) {

        printHeader("Kirim Pesanan");

        if (!adaPesananMenungguKurir()) {

            cout << "\nTidak ada orderan aktif.\n";
        }
        else {

            tampilRadarOrderan();
            kirimPesanan();
        }

        cout << "\nTekan Enter untuk kembali...";

        cin.ignore();
        cin.get();
    }

        else if (pilihan == 3) {

        printHeader("Selesaikan Pengiriman");

        if (!adaPesananDikirim()) {

            cout << "\nTidak ada orderan aktif.\n";
        }
        else {

            tampilRadarOrderan();
            selesaikanPengiriman();
        }

        cout << "\nTekan Enter untuk kembali...";

        cin.ignore();
        cin.get();
    }

    else if (pilihan == 4) {

    printHeader("Navigasi Kurir");

    navigasiDijkstra();

    cout << "\nTekan Enter untuk kembali...";

    cin.ignore();
    cin.get();
}

        else if (pilihan == 0) {
            break;
        }
    }
}

void MenuAwal() {
    for (int i = 0; i < 10; i++) {
        rootMenu = insertMenu(rootMenu, daftarMenu[i].id, daftarMenu[i].nama, daftarMenu[i].harga, daftarMenu[i].kategori);
    }
}

int login(string roleName, string correctUser, string correctPass) {
    string username, password;
    cout << "\n--- Login " << roleName << " ---\n";
    cout << "Username: "; cin >> username;
    cout << "Password: "; cin >> password;
    
    if (username == correctUser && password == correctPass) {
        return 1;
    }
    
    cout << "Login gagal! Username atau password salah.\n";
    return 0;
}

void jalankanSistem() {
    MenuAwal();
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
                if (login("Pemesan", "pemesan", "123") == 1) {
                    layarPemesan();
                } else {
                    cout << "Tekan Enter untuk kembali...";
                    cin.ignore(); cin.get();
                }
                break;
            case 2:
                if (login("Admin Resto", "admin", "123") == 1) {
                    layarResto();
                } else {
                    cout << "Tekan Enter untuk kembali...";
                    cin.ignore(); cin.get();
                }
                break;
            case 3:
                if (login("Kurir", "kurir", "123") == 1) {
                    layarKurir();
                } else {
                    cout << "Tekan Enter untuk kembali...";
                    cin.ignore(); cin.get();
                }
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