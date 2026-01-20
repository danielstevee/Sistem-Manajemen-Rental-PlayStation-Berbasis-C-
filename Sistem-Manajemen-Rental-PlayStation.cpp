#include <iostream>
#include <string>
#include <unordered_map>
#include <functional> 
using namespace std;

const int HARGA_PER_JAM = 5000;
const string ADMIN_USERNAME = "admin";
const string ADMIN_PASSWORD = "admin123";

const int MAX_GAMES = 10;
const int MAX_REKOMENDASI = 5;

struct SewaNode
{                      // untuk member
    string namaKonsol; // Nama konsol yang disewa
    int durasiJam;     // Durasi sewa dalam jam
    int totalBayar;    // Total biaya sewa
    SewaNode *next;    // Pointer ke node berikutnya
};

struct GameList
{
    string games[10];
    int jumlahGame;
};

struct UserNode
{
    string nama;         // Nama pengguna
    string username;     // Username unik
    string password;     // Password pengguna
    string nomorHP;      // Nomor HP pengguna
    bool statusMember;   // Status membership
    int jenisMembership; // Jenis membership
    int hariMember;      // Jumlah hari member aktif
    SewaNode *headSewa;
    UserNode *next; // Pointer ke node berikutnya dalam linked list
    UserNode *prev; // Pointer ke node sebelumnya dalam linked list
};

struct KonsolPS
{
    string nama;
    bool tersedia;
    int hargaPerJam;
    int totalJamDisewa;
    int idPenyewa;
};

struct Pelanggan
{
    int id;
    string nama;
    int durasiJam;
    int totalBayar;
    bool sedangMenyewa;
};

struct KonsolNode
{
    KonsolPS data;
    KonsolNode *left;
    KonsolNode *right;
};

struct PelangganNode
{
    Pelanggan data;
    PelangganNode *left;
    PelangganNode *right;
};
UserNode *headUser = nullptr;
KonsolNode *rootKonsol = nullptr;
PelangganNode *rootPelanggan = nullptr;
unordered_map<string, GameList> gameKonsol;
unordered_map<string, GameList> gameGenre; // Graph untuk menyimpan game berdasarkan genre
unordered_map<string, GameList> gameNew;   // Hash table untuk menyimpan game baru khusus member

// Deklarasi fungsi
// Deklarasi fungsi
void mainMenu(KonsolNode *root);
void menuAdmin(KonsolNode *root, PelangganNode *rootPelanggan, UserNode *headUser,
               unordered_map<string, GameList> &gameKonsol, unordered_map<string, GameList> &gameGenre);
void usermember(KonsolNode *root, UserNode *headUser);
void login(KonsolNode *root, UserNode *headUser);
void daftar(KonsolNode *root);
void MenuUserMember(UserNode *user, unordered_map<string, GameList> &gameNew);
void MenuAlternatif(KonsolNode *root, UserNode *headUser, unordered_map<string, GameList> &gameKonsol);
void customerMenu(KonsolNode *root);


// menyimpan data konsol dengan tree
KonsolNode *cariKonsol(KonsolNode *root, string namaKonsol)
{
    if (root == nullptr)
        return nullptr;
    if (namaKonsol < root->data.nama)
        return cariKonsol(root->left, namaKonsol);
    if (namaKonsol > root->data.nama)
        return cariKonsol(root->right, namaKonsol);
    return root; 

}



// simpan data pelanggan ke tree
PelangganNode *insertPelanggan(PelangganNode *root, Pelanggan pelanggan)
{
    if (root == nullptr)
    {
        return new PelangganNode{pelanggan, nullptr, nullptr};
    }
    if (pelanggan.id < root->data.id)
    {
        root->left = insertPelanggan(root->left, pelanggan);
    }
    else
    {
        root->right = insertPelanggan(root->right, pelanggan);
    }
    return root;
}

// cari pelanggan dari tree menggunakan ID 
PelangganNode *cariPelanggan(PelangganNode *root, int id)
{
    if (root == nullptr)
        return nullptr;
    if (id < root->data.id)
        return cariPelanggan(root->left, id);
    else if (id > root->data.id)
        return cariPelanggan(root->right, id);
    else
        return root; // ketemu
}

// simpan data konsol ke tree
KonsolNode *insertKonsol(KonsolNode *root, KonsolPS konsol)
{
    if (root == nullptr)
    {
        return new KonsolNode{konsol, nullptr, nullptr};
    }
    if (konsol.nama < root->data.nama)
    {
        root->left = insertKonsol(root->left, konsol);
    }
    else
    {
        root->right = insertKonsol(root->right, konsol);
    }
    return root;
}

//hapus sewa menggunakan linklist dgn menunjuk data user
void hapusSewa(UserNode *user, const string &namaKonsol)
{
    if (user->headSewa == nullptr)
    {
        cout << "Tidak ada data sewa untuk dikembalikan.\n";
        return;
    }

    SewaNode *temp = user->headSewa;
    SewaNode *prev = nullptr;

    // Cari node yang sesuai dengan nama konsol
    while (temp != nullptr && temp->namaKonsol != namaKonsol)
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == nullptr)
    {
        cout << "Konsol " << namaKonsol << " tidak ditemukan dalam daftar sewa.\n";
        return;
    }

    // Hapus node dari linked list
    if (prev == nullptr)
    {
        user->headSewa = temp->next; // Node pertama dihapus
    }
    else
    {
        prev->next = temp->next; // Node di tengah atau akhir dihapus
    }

    delete temp; // Bebaskan memori
    cout << "Konsol " << namaKonsol << " berhasil dikembalikan.\n";
}


//menyimpan data sewa dengan linklist 
void tambahSewa(UserNode *user, const string &namaKonsol, int durasiJam, int totalBayar)
{
    SewaNode *newSewa = new SewaNode{namaKonsol, durasiJam, totalBayar, nullptr};

    if (user->headSewa == nullptr)
    {
        user->headSewa = newSewa; // Jika linked list kosong, jadikan node baru sebagai head
    }
    else
    {
        SewaNode *temp = user->headSewa;
        while (temp->next != nullptr)
        {
            temp = temp->next; // Cari node terakhir
        }
        temp->next = newSewa; // Tambahkan node baru di akhir
    }
}


//menyimpan data user dengan linklist
void tambahUser(string nama, string username, string password, string nomorHP, bool statusMember, int jenisMembership, int hariMember)
{
    UserNode *newUser = new UserNode{nama, username, password, nomorHP, true, jenisMembership, hariMember, nullptr};
    if (headUser == nullptr)
    {
        headUser = newUser; // Jika linked list kosong, jadikan node baru sebagai head
    }
    else
    {
        UserNode *temp = headUser;
        while (temp->next != nullptr)
        {
            temp = temp->next; // Cari node terakhir
        }
        temp->next = newUser; // Tambahkan node baru di akhir
        newUser->prev = temp;
    }
}

//mengembalikan konsol dgn memperbarui data tree
void KembalikanKonsol(KonsolNode *root, string namaKonsol)
{
    KonsolNode *konsol = cariKonsol(root, namaKonsol);
    if (konsol == nullptr)
    {
        cout << "Konsol tidak ditemukan.\n";
        mainMenu(root);
    }

    if (konsol->data.tersedia)
    {
        cout << "Konsol sudah dalam keadaan tersedia. Tidak perlu dikembalikan.\n";
        mainMenu(root);
    }

    konsol->data.tersedia = true;
    konsol->data.totalJamDisewa = 0;

    // Cari pelanggan berdasarkan ID penyewa
    PelangganNode *pelangganNode = cariPelanggan(rootPelanggan, konsol->data.idPenyewa);
    if (pelangganNode != nullptr)
    {
        pelangganNode->data.sedangMenyewa = false;
        cout << "Konsol " << konsol->data.nama << " telah dikembalikan.\n";
        cout << "Status sewa pelanggan " << pelangganNode->data.nama << " telah diubah menjadi selesai.\n";
    }
    else
    {
        cout << "Pelanggan penyewa tidak ditemukan.\n";
        mainMenu(root);
    }

    // Reset ID penyewa karena konsol sudah dikembalikan
    konsol->data.idPenyewa = 0;

    mainMenu(root);
}

//sewa konsol dgn memperbarui data tree
void SewaKonsol(KonsolNode *root, string namaKonsol, Pelanggan &pelanggan)
{
    KonsolNode *konsol = cariKonsol(root, namaKonsol);
    if (konsol == nullptr)
    {
        cout << "Konsol tidak ditemukan.\n";
        mainMenu(root);
    }

    if (konsol->data.tersedia)
    {
        cout << "Konsol " << konsol->data.nama << " tersedia.\n";
        cout << "Masukkan durasi sewa dalam jam: ";
        int durasi;
        cin >> durasi;
        pelanggan.totalBayar = durasi * konsol->data.hargaPerJam;
        pelanggan.durasiJam = durasi;
        pelanggan.sedangMenyewa = true;
        konsol->data.tersedia = false;
        konsol->data.totalJamDisewa += durasi;

        cout << "Masukkan nama penyewa: ";
        cin.ignore();
        getline(cin, pelanggan.nama);

        // Assign ID otomatis
        if (rootPelanggan == nullptr)
        {
            pelanggan.id = 1;
        }
        else
        {
            PelangganNode *temp = rootPelanggan;
            while (temp->right != nullptr)
            {
                temp = temp->right;
            }
            pelanggan.id = temp->data.id + 1;
        }

        konsol->data.idPenyewa = pelanggan.id; // <-- simpan ID penyewa ke konsol
        rootPelanggan = insertPelanggan(rootPelanggan, pelanggan);

        cout << "ID penyewa: " << pelanggan.id << endl;
        cout << "Total billing: Rp " << pelanggan.totalBayar << endl;
        cout << "Konsol " << konsol->data.nama << " berhasil disewa oleh " << pelanggan.nama << ".\n";
        mainMenu(root);
    }
    else
    {
        cout << "Konsol sedang disewa.\n";
        mainMenu(root);
    }
}

void mainMenu(KonsolNode *root)
{
    cout << "======================\n";
    cout << "    MENU PELANGGAN    \n";
    cout << "======================\n";
    cout << "1. Sewa Konsol\n";     //tree
    cout << "2. Kembalikan Konsol\n"; //tree
    cout << "3. Cari Data Pelanggan\n"; //searching
    cout << "4. Lihat Game Konsol\n"; // hashtable 1 dgn graph
    cout << "5. User Member\n";       //linklist,linklist, graph, sorting, 
    cout << "6. Keluar\n";
    cout << "======================\n";
    cout << "Masukkan pilihan (1/2/3/4/5/6): ";
    int pilihan;
    cin >> pilihan;
    switch (pilihan)
    {
    case 1:
    {
        string namaKonsol;
        Pelanggan pelanggan;
        cout << "Masukkan nama konsol yang ingin disewa: ";
        cin >> namaKonsol;
        SewaKonsol(root, namaKonsol, pelanggan);
        break;
    }
    case 2:
    {
        string namaKonsol;
        cout << "Masukkan nama konsol yang ingin dikembalikan: ";
        cin >> namaKonsol;
        KembalikanKonsol(root, namaKonsol);
        break;
    }
    case 3:
    {
        int idCari;
        cout << "Masukkan ID pelanggan: ";
        cin >> idCari;
        PelangganNode *hasil = cariPelanggan(rootPelanggan, idCari);
        if (hasil)
        {
            cout << "Data ditemukan:\n";
            cout << "Nama: " << hasil->data.nama << endl;
            cout << "Durasi sewa: " << hasil->data.durasiJam << " jam\n";
            cout << "Total bayar: Rp " << hasil->data.totalBayar << endl;
            cout << "Status sewa: " << (hasil->data.sedangMenyewa ? "Masih menyewa" : "Selesai") << endl;
        }
        else
        {
            cout << "Pelanggan dengan ID tersebut tidak ditemukan.\n";
        }
        mainMenu(root);
        break;
    }
    case 4:
    {
        string input;
        cout << "Masukkan jenis konsol untuk melihat game (PS1/PS2/PS3/PS4): ";
        cin >> input;
        gameKonsol[input];
        if (gameKonsol.find(input) != gameKonsol.end())
        {
            cout << "Game di " << input << ":\n";
            for (int i = 0; i < gameKonsol[input].jumlahGame; i++)
            {
                cout << "- " << gameKonsol[input].games[i] << endl;
            }
            mainMenu(root);
        }
        else
        {
            cout << "Konsol tidak ditemukan.\n";
            mainMenu(root);
        }
        break;
    }
    case 5:
        usermember(root, headUser);
        break;

    case 6:
        MenuAlternatif(rootKonsol, headUser, gameKonsol);
        break;
    default:
        cout << "Pilihan tidak valid. Silakan pilih lagi.\n";
        break;
    }
}

void loginAdmin(KonsolNode *root, UserNode *headUser, unordered_map<string, GameList> &gameKonsol)
{
    string usernameAdmin, passwordAdmin;

    cout << "==========================\n";
    cout << "|      LOGIN ADMIN       |\n";
    cout << "==========================\n";
    cout << "Masukkan Username Admin: ";
    cin >> usernameAdmin;
    cout << "Masukkan Password Admin: ";
    cin >> passwordAdmin;

    // Validasi username dan password
    if (usernameAdmin == ADMIN_USERNAME && passwordAdmin == ADMIN_PASSWORD)
    {
        cout << "Login berhasil! Selamat datang, Admin.\n";
        menuAdmin(root, rootPelanggan, headUser, gameKonsol, gameGenre); // Masuk ke menu admin
    }
    else
    {
        cout << "Login gagal! Username atau password salah.\n";
        loginAdmin(root, headUser, gameKonsol); // Panggil ulang login admin jika gagal
    }
}

void tampilkanSemuaPelanggan(PelangganNode *root)
{
    if (root == nullptr)
    {
        cout << "Tidak ada data pelanggan.\n";
        return;
    }

    // Traversal inorder untuk menampilkan data pelanggan secara terurut
    tampilkanSemuaPelanggan(root->left);
    cout << "ID: " << root->data.id << endl;
    cout << "Nama: " << root->data.nama << endl;
    cout << "Durasi Sewa: " << root->data.durasiJam << " jam" << endl;
    cout << "Total Bayar: Rp " << root->data.totalBayar << endl;
    cout << "Status Sewa: " << (root->data.sedangMenyewa ? "Masih menyewa" : "Selesai") << endl;
    cout << "-----------------------------" << endl;
    tampilkanSemuaPelanggan(root->right);

}

void tampilkanMember(UserNode *headUser)
{
    if (headUser == nullptr)
    {
        cout << "Tidak ada data member yang tersedia.\n";
        return;
    }

    UserNode *current = headUser;

    while (true)
    {
        // Menampilkan data member saat ini
        cout << "===========================================\n";
        cout << "|             ~ DATA MEMBER ~             |\n";
        cout << "===========================================\n";
        cout << "| Username     : " << current->username << "\n";
        cout << "| Nama         : " << current->nama << "\n";
        cout << "| Nomor HP     : " << current->nomorHP << "\n";
        cout << "| Hari Member  : " << current->hariMember << "\n";
        cout << "| Status       : " << (current->statusMember ? "Aktif" : "Nonaktif") << "\n";
        cout << "===========================================\n";

        // Menu navigasi
        cout << "Ketik 1 untuk 'next'\n";
        cout << "Ketik 2 untuk 'prev' \n";
        cout << "Ketik 'nonaktif' untuk menonaktifkan status member\n";
        cout << "Ketik 'exit' untuk keluar\n";
        cout << "Masukkan pilihan: ";
        string pilihan;
        cin >> pilihan;

        if (pilihan == "1")
        {
            // Navigasi ke data berikutnya
            if (current->next != nullptr)
            {
                current = current->next;
            }
            else
            {
                cout << "Ini adalah data terakhir.\n";
            }
        }
        else if (pilihan == "2")
        {
            // Navigasi ke data sebelumnya
            if (current->prev != nullptr)
            {
                current = current->prev;
            }
            else
            {
                cout << "Ini adalah data pertama.\n";
            }
        }
        else if (pilihan == "nonaktif")
        {
            // Menonaktifkan status member
            if (current->statusMember)
            {
                current->statusMember = false;
                cout << "Status member berhasil diubah menjadi Nonaktif.\n";
                break;
            }
            else
            {
                cout << "Status member sudah Nonaktif.\n";
            }
        }
        else if (pilihan == "exit")
        {
            // Keluar dari menu
            cout << "Keluar dari menu tampilkan member.\n";
            break;
        }
        else
        {
            cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }
    }
}

void tampilkanGameGenre(unordered_map<string, GameList> &gameGenre)
{
    cout << "=========================================\n";
    cout << "|              DAFTAR GAME              |\n";
    cout << "=========================================\n";

    for (const auto &pair : gameGenre)
    {
        const string &genre = pair.first;       // Nama genre
        const GameList &gameList = pair.second; // Daftar game untuk genre tersebut

        cout << "Genre: " << genre << "\n";
        cout << "-----------------------------------------\n";
        for (int i = 0; i < gameList.jumlahGame; ++i)
        {
            cout << "- " << gameList.games[i] << "\n";
        }
        cout << "=========================================\n";
    }
         // Tambahkan ini untuk kembali ke menu admin
    cout << "Tekan Enter untuk kembali ke menu admin...";
    cin.ignore();
    cin.get();
    menuAdmin(rootKonsol, rootPelanggan, headUser, gameKonsol, gameGenre);
}

// Fungsi untuk mendapatkan harga membership berdasarkan jenis
int getHargaMembership(int jenisMembership) {
    switch (jenisMembership) {
        case 1: return 7000;      // Harian
        case 2: return 30000;     // Mingguan  
        case 3: return 100000;    // Bulanan
        case 4: return 1000000;   // Tahunan
        case 5: return 1500000;   // Lifetime
        default: return 0;
    }
}

// Fungsi untuk menghitung pendapatan dari tree pelanggan (non-member)
void hitungPendapatanNonMember(PelangganNode *root, int &total) {
    if (root == nullptr) return;
    
    hitungPendapatanNonMember(root->left, total);
    total += root->data.totalBayar;  // Tambahkan total bayar dari setiap pelanggan
    hitungPendapatanNonMember(root->right, total);
}

// Fungsi untuk menghitung pendapatan dari linked list member
void hitungPendapatanMember(UserNode *headUser, int &totalMembership, int &totalSewaMember) {
    UserNode *current = headUser;
    while (current != nullptr) {
        if (current->statusMember) {
            // Hitung pendapatan dari membership
            totalMembership += getHargaMembership(current->jenisMembership);
            
            // Hitung pendapatan dari sewa konsol oleh member
            SewaNode *sewa = current->headSewa;
            while (sewa != nullptr) {
                totalSewaMember += sewa->totalBayar;
                sewa = sewa->next;
            }
        }
        current = current->next;
    }
}

void laporanKeuangan(PelangganNode *rootPelanggan, UserNode *headUser) {
    int totalPendapatanNonMember = 0;
    int totalPendapatanMembership = 0;
    int totalPendapatanSewaMember = 0;

    // Hitung pendapatan dari non-member (data tree)
    hitungPendapatanNonMember(rootPelanggan, totalPendapatanNonMember);

    // Hitung pendapatan dari member (data linked list)
    hitungPendapatanMember(headUser, totalPendapatanMembership, totalPendapatanSewaMember);

    // Tampilkan laporan keuangan
    cout << "=========================================\n";
    cout << "|          LAPORAN KEUANGAN            |\n";
    cout << "=========================================\n";
    cout << "Pendapatan dari Non-Member: Rp " << totalPendapatanNonMember << "\n";
    cout << "Pendapatan dari Membership: Rp " << totalPendapatanMembership << "\n";
    cout << "Pendapatan dari Sewa Member: Rp " << totalPendapatanSewaMember << "\n";
    cout << "-----------------------------------------\n";
    cout << "Total Pendapatan: Rp " << (totalPendapatanNonMember + totalPendapatanMembership + totalPendapatanSewaMember) << "\n";
    cout << "=========================================\n";
}

void menuAdmin(KonsolNode *root, PelangganNode *rootPelanggan, UserNode *headUser, unordered_map<string, GameList> &gameKonsol, unordered_map<string, GameList> &gameGenre)
{
    cout << "==========================\n";
    cout << "|     ~ MENU ADMIN ~     |\n";
    cout << "==========================\n";
    cout << "| 1. Tampilkan Nonmember |\n"; //linked list
    cout << "| 2. Tampilkan Member    |\n"; //linked list
    cout << "| 3. Lihat Seluruh Game  |\n"; //graph
    cout << "| 4. Laporan Keuangan    |\n"; //sorting
    cout << "| 5. Keluar              |\n"; // Kembali ke menu utama
    cout << "==========================\n";
    cout << "Masukkan Pilihan (1/2/3/4/5): ";

    int pilihan;
    cin >> pilihan;

    switch (pilihan)
    {
    case 1:
        cout << "Menampilkan Data Pelanggan Nonmember :\n";
        tampilkanSemuaPelanggan(rootPelanggan); // Fungsi untuk menampilkan data nonmember dari linked list
                 // Tambahkan ini untuk kembali ke menu admin
    cout << "Tekan Enter untuk kembali ke menu admin...";
    cin.ignore();
    cin.get();
    menuAdmin(rootKonsol, rootPelanggan, headUser, gameKonsol, gameGenre);
        break;
    case 2:
        cout << "Menampilkan Member (Linked List):\n";
        tampilkanMember(headUser); // Fungsi untuk menampilkan data member dari linked list
                 // Tambahkan ini untuk kembali ke menu admin
    cout << "Tekan Enter untuk kembali ke menu admin...";
    cin.ignore();
    cin.get();
    menuAdmin(rootKonsol, rootPelanggan, headUser, gameKonsol, gameGenre);
        break;
    case 3:
        cout << "Menampilkan Seluruh Game (Hash Table):\n";
        tampilkanGameGenre(gameGenre); // Fungsi untuk menampilkan seluruh game dari hash table
        break;
    case 4:
        cout << "Laporan Keuangan (Sorting):\n";
        laporanKeuangan(rootPelanggan, headUser); // Fungsi untuk menampilkan laporan keuangan dengan sorting
                 // Tambahkan ini untuk kembali ke menu admin
    cout << "Tekan Enter untuk kembali ke menu admin...";
    cin.ignore();
    cin.get();
    menuAdmin(rootKonsol, rootPelanggan, headUser, gameKonsol, gameGenre);
        break;
    case 5:
        cout << "Keluar dari Menu Admin.\n";
        MenuAlternatif(rootKonsol, headUser, gameKonsol); // Kembali ke menu utama
        break;                                            // Keluar dari menu admin
    default:
        cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        menuAdmin(rootKonsol, rootPelanggan, headUser, gameKonsol, gameGenre);
        // Panggil ulang menu admin jika input tidak valid
    }
}

void MenuAlternatif(KonsolNode *rootKonsol, UserNode *headUser, unordered_map<string, GameList> &gameKonsol)
{ // alternatif agar memanggil seperti main fungsinya
    string input;
    cout << "====================\n";
    cout << "|  Selamat Datang  |\n";
    cout << "====================\n";
    cout << "|1. Admin          |\n";
    cout << "|2. Customer       |\n";
    cout << "|3. Keluar         |\n";
    cout << "====================\n";
    cout << "Masukkan pilihan:  \n";
    cin >> input;

    if (input == "1")
    {
        loginAdmin(rootKonsol, headUser, gameKonsol);
    }
    else if (input == "2")
    {
        customerMenu(rootKonsol);
    } else if (input == "3")
    {
        cout << "Terimakasih :)";
    }
    else
    {  //rekursif karena memanggil diri nya sendiri menggunakan kata kunci "MenuAlternatif" yang menyimpan data tree,linklist,dan hashtable, serta graph
        cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        MenuAlternatif(rootKonsol, headUser, gameKonsol);
    }
}

void customerMenu(KonsolNode *root)
{
    cout << "======================\n";
    cout << "    MENU PELANGGAN    \n";
    cout << "======================\n";
    cout << "1. Sewa Konsol\n";
    cout << "2. Kembalikan Konsol\n";
    cout << "3. Cari Data Pelanggan\n";
    cout << "4. Lihat Game Konsol\n"; // hashtable 1 dgn graph
    cout << "5. User Member\n";
    cout << "6. Keluar\n";
    cout << "======================\n";
    cout << "Masukkan pilihan (1/2/3/4/5/6): ";
    int pilihan;
    cin >> pilihan;
    switch (pilihan)
    {
    case 1:
    {
        string namaKonsol;
        Pelanggan pelanggan;
        cout << "Masukkan nama konsol yang ingin disewa: ";
        cin >> namaKonsol;
        SewaKonsol(root, namaKonsol, pelanggan);
        break;
    }
    case 2:
    {
        string namaKonsol;
        cout << "Masukkan nama konsol yang ingin dikembalikan: ";
        cin >> namaKonsol;
        KembalikanKonsol(root, namaKonsol);
        break;
    }
    case 3:
    {
        int idCari;
        cout << "Masukkan ID pelanggan: ";
        cin >> idCari;
        PelangganNode *hasil = cariPelanggan(rootPelanggan, idCari);
        if (hasil)
        {
            cout << "Data ditemukan:\n";
            cout << "Nama: " << hasil->data.nama << endl;
            cout << "Durasi sewa: " << hasil->data.durasiJam << " jam\n";
            cout << "Total bayar: Rp " << hasil->data.totalBayar << endl;
            cout << "Status sewa: " << (hasil->data.sedangMenyewa ? "Masih menyewa" : "Selesai") << endl;
        }
        else
        {
            cout << "Pelanggan dengan ID tersebut tidak ditemukan.\n";
        }
        customerMenu(root);
        break;
    }
    case 4:
    {
        string input;
        cout << "Masukkan jenis konsol untuk melihat game (PS1/PS2/PS3/PS4): ";
        cin >> input;
        gameKonsol[input];
        if (gameKonsol.find(input) != gameKonsol.end())
        {
            cout << "Game di " << input << ":\n";
            for (int i = 0; i < gameKonsol[input].jumlahGame; i++)
            {
                cout << "- " << gameKonsol[input].games[i] << endl;
            }
            customerMenu(root);
        }
        else
        {
            cout << "Konsol tidak ditemukan.\n";
            customerMenu(root);
        }
        break;
    }
    case 5:
        usermember(root, headUser);
        break;

    case 6:
        MenuAlternatif(rootKonsol, headUser, gameKonsol);
        break;
    default:
        cout << "Pilihan tidak valid. Silakan pilih lagi.\n";
        customerMenu(root);
        break;
    }
}

void usermember(KonsolNode *root, UserNode *headUser)
{
    cout << "================\n";
    cout << "|  User Member |\n";
    cout << "================\n";
    cout << "|1. Login      |\n";
    cout << "|2. Daftar     |\n";
    cout << "|3. Kembali    |\n";
    cout << "================\n";
    cout << "Masukan Pilihan: ";
    int pilihan;
    cin >> pilihan;
    switch (pilihan)
    {
    case 1:
        login(root, headUser);
        break;
    case 2:
        daftar(root);
        break;
    case 3:
        mainMenu(root);
        break;
    default:
        cout << "Pilihan tidak valid. Silakan pilih lagi.\n";
        usermember(root, headUser);
        break;
    }
}

void login(KonsolNode *root, UserNode *headUser)
{
    string username, password;
    cout << "Masukkan Username: ";
    cin >> username;
    cout << "Masukkan Password: ";
    cin >> password;

    UserNode *temp = headUser;
    while (temp != nullptr)
    {
        if (temp->username == username && temp->password == password)
        {
            cout << "Login berhasil! Selamat datang, " << temp->nama << ".\n";
            MenuUserMember(temp, gameNew);
            // Keluar dari fungsi jika login berhasil
            return;
        }
        temp = temp->next; // Lanjutkan ke node berikutnya
    }

    cout << "Login gagal! Username atau password salah.\n";
    cout << "Silakan coba lagi/ ketik 1 untuk kembali ke customer menu\n";
    int pilihan;
    cin >> pilihan;
    if (pilihan == 1)
    {
        customerMenu(root);
    }
    return;
}

void tampilkanGameNew(unordered_map<string, GameList> &gameNew)
{
    cout << "=========================================\n";
    cout << "|       DAFTAR GAME BARU UNTUK MEMBER   |\n";
    cout << "=========================================\n";

    for (const auto &pair : gameNew)
    {
        const string &konsol = pair.first;      // Nama konsol
        const GameList &gameList = pair.second; // Daftar game untuk konsol tersebut

        cout << "Konsol: " << konsol << "\n";
        cout << "-----------------------------------------\n";
        for (int i = 0; i < gameList.jumlahGame; ++i)
        {
            cout << "- " << gameList.games[i] << "\n";
        }
        cout << "=========================================\n";
    }
    MenuUserMember(headUser, gameNew);
}

void sewaKonsol(UserNode *user, unordered_map<string, GameList> &gameNew)
{
    cout << "==========================\n";
    cout << "|      Sewa Konsol       |\n";
    cout << "==========================\n";

    // Tampilkan daftar konsol yang tersedia
    cout << "Daftar Konsol yang Tersedia:\n";
    cout << "1. PS1\n";
    cout << "2. PS2\n";
    cout << "3. PS3\n";
    cout << "4. PS4\n";
    cout << "Masukkan Pilihan Konsol (1/2/3/4): ";
    int pilihanKonsol;
    cin >> pilihanKonsol;

    string namaKonsol;
    switch (pilihanKonsol)
    {
    case 1:
        namaKonsol = "PS1";
        break;
    case 2:
        namaKonsol = "PS2";
        break;
    case 3:
        namaKonsol = "PS3";
        break;
    case 4:
        namaKonsol = "PS4";
        break;
    default:
        cout << "Pilihan tidak valid.\n";
        return;
    }

    // Masukkan durasi sewa
    cout << "Masukkan Durasi Sewa (dalam jam): ";
    int durasiJam;
    cin >> durasiJam;

    // Hitung total biaya sewa
    int hargaPerJam = 5000; // Contoh harga per jam
    int totalBayar = durasiJam * hargaPerJam;

    // Tambahkan data sewa ke linked list user
    tambahSewa(user, namaKonsol, durasiJam, totalBayar);

    cout << "Sewa berhasil! Detail:\n";
    cout << "Konsol: " << namaKonsol << "\n";
    cout << "Durasi: " << durasiJam << " jam\n";
    cout << "Total Bayar: Rp " << totalBayar << "\n";
    MenuUserMember(user, gameNew); // Kembali ke menu user member setelah sewa berhasil
}

void kembalikanKonsolMember(UserNode *user)
{
    if (user->headSewa == nullptr)
    {
        cout << "Tidak ada konsol yang sedang disewa.\n";
        MenuUserMember(user, gameNew);
        return;
    }

    cout << "Daftar Konsol yang Sedang Disewa:\n";
    SewaNode *temp = user->headSewa;
    while (temp != nullptr)
    {
        cout << "- " << temp->namaKonsol << " (Durasi: " << temp->durasiJam << " jam, Total Bayar: Rp " << temp->totalBayar << ")\n";
        temp = temp->next;
    }

    cout << "Masukkan nama konsol yang ingin dikembalikan: ";
    string namaKonsol;
    cin.ignore(); // Membersihkan buffer input
    getline(cin, namaKonsol);

    // Hapus data sewa dari linked list user
    hapusSewa(user, namaKonsol);
    MenuUserMember(user, gameNew);
}

void MenuUserMember(UserNode *user, unordered_map<string, GameList> &gameNew)
{
    cout << "====================================\n";
    cout << " User Member: " << user->username << "\n";
    cout << "====================================\n";
    cout << "| 1. Sewa Konsol (PROMO)           |\n";
    cout << "| 2. Kembalikan Konsol             |\n";
    cout << "| 3. Lihat Game (NEW)              |\n";
    cout << "| 4. Log out                       |\n";
    cout << "====================================\n";
    cout << "Masukkan Pilihan: (1/2/3/4) ";

    int pilihan;
    cin >> pilihan;

    switch (pilihan)
    {
    case 1:
        cout << "Fitur Sewa Konsol dipilih.\n";
        sewaKonsol(user, gameNew);
        break;
    case 2:
        cout << "Fitur Kembalikan Konsol dipilih.\n";
        kembalikanKonsolMember(user);
        break;
    case 3:
        cout << "Fitur Lihat Game dipilih.\n";
        tampilkanGameNew(gameNew);
        break;
    case 4:
        cout << "Log out berhasil. Kembali ke menu utama.\n";
        usermember(rootKonsol, headUser);
        break;
    default:
        cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        MenuUserMember(user, gameNew); // Panggil ulang menu jika input tidak valid
        break;
    }
}
void daftar(KonsolNode *root)
{
    string nama, username, password, nomorHP;
    int jenisMembership = 0; // Default jenis membership
    int hariMember = 0;

    cout << "Masukkan Nama: ";
    cin.ignore(); // Membersihkan buffer input
    getline(cin, nama);

    cout << "Masukkan Username (unik): ";
    cin >> username;

    // Periksa apakah username sudah ada
    UserNode *temp = headUser;
    while (temp != nullptr)
    {
        if (temp->username == username)
        {
            cout << "Username sudah digunakan. Silakan coba lagi.\n";
            return daftar(root); // Meminta input ulang tanpa rekursi berlebihan
        }
        temp = temp->next;
    }

    cout << "Masukkan Password: ";
    cin >> password;

    cout << "Masukkan Nomor HP: ";
    cin >> nomorHP;

    cout << "Status Masa Aktif Member:\n";
    cout << "==================================================================================================\n";
    cout << "| Jenis Membership | Masa Aktif | Harga Umum   | Fasilitas Umum                                  |\n";
    cout << "==================================================================================================\n";
    cout << "| 1. Harian        | 1 hari     | Rp 7.000     | Akses 2–3 jam, tanpa login ulang                |\n";
    cout << "| 2. Mingguan      | 7 hari     | Rp 30.000    | Bonus 1 jam/hari, login otomatis                |\n";
    cout << "| 3. Bulanan       | 30 hari    | Rp 100.000   | Diskon per jam (3.000/jam), bebas antre         |\n";
    cout << "| 4. Tahunan       | 365 hari   | Rp 1.000.000 | Akses premium, ruang VIP, saldo awal gratis     |\n";
    cout << "| 5. Lifetime      | Selamanya  | Rp 1.500.000 | Promo langka, untuk loyal user                  |\n";
    cout << "==================================================================================================\n";
    cout << "Masukkan Pilihan (1/2/3/4/5): ";
    cin >> jenisMembership;

    // Tentukan jumlah hari berdasarkan jenis membership
    switch (jenisMembership)
    {
    case 1:
        hariMember = 1;
        break;
    case 2:
        hariMember = 7;
        break;
    case 3:
        hariMember = 30;
        break;
    case 4:
        hariMember = 365;
        break;
    case 5:
        hariMember = -1; // Lifetime (tidak terbatas)
        break;
    default:
        cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        return daftar(root); // Meminta input ulang jika pilihan tidak valid
    }

    // Tambahkan user baru ke linked list
    tambahUser(nama, username, password, nomorHP, true, jenisMembership, hariMember);
    cout << "Pendaftaran berhasil! Silakan login.\n";
    usermember(root, headUser); // Kembali ke menu user member setelah pendaftaran
}

void isiDataGameNew()
{
    gameNew["PS1"] = {{"Final Fantasy VII", "Metal Gear Solid", "Silent Hill"}, 3};
    gameNew["PS2"] = {{"Kingdom Hearts", "Shadow of the Colossus", "Devil May Cry"}, 3};
    gameNew["PS3"] = {{"The Last of Us", "Uncharted 3", "Red Dead Redemption"}, 3};
    gameNew["PS4"] = {{"Horizon Zero Dawn", "Ghost of Tsushima", "Cyberpunk 2077"}, 3};
}

void isiDataGameGenre()
{
    gameGenre["Racing"] = {{"Gran Turismo", "Need for Speed", "Crash Bandicoot"}, 3};
    gameGenre["Action"] = {{"Spider-Man", "God of War", "Horizon Zero Dawn"}, 3};
    gameGenre["Adventure"] = {{"Final Fantasy VII", "Uncharted 3", "Harvest Moon"}, 3};
    gameGenre["Horror"] = {{"Silent Hill", "Resident Evil 4", "The Last of Us"}, 3};
    gameGenre["RPG"] = {{"Kingdom Hearts", "Cyberpunk 2077", "Red Dead Redemption"}, 3};
}

int main()
{
    // Inisialisasi konsol
    rootKonsol = insertKonsol(rootKonsol, {"PS1", true, HARGA_PER_JAM, 0});
    rootKonsol = insertKonsol(rootKonsol, {"PS2", true, HARGA_PER_JAM, 0});
    rootKonsol = insertKonsol(rootKonsol, {"PS3", true, HARGA_PER_JAM, 0});
    rootKonsol = insertKonsol(rootKonsol, {"PS4", true, HARGA_PER_JAM, 0});

    gameKonsol["PS1"] = {{"Crash Bandicoot", "Gran Turismo", "Tekken 3"}, 3};
    gameKonsol["PS2"] = {{"Super Mario", "Need for Speed", "Harvest Moon"}, 3};
    gameKonsol["PS3"] = {{"GTA San Andreas", "God of War", "Resident Evil 4"}, 3};
    gameKonsol["PS4"] = {{"Spider-Man", "God of War Ragnarok", "GTA VI"}, 3};

    // untuk game genre (admin)
    isiDataGameGenre();

    // untuk game baru (member)
    isiDataGameNew();

    string input;
    cout << "====================\n";
    cout << "|  Selamat Datang  |\n";
    cout << "====================\n";
    cout << "|1. Admin          |\n";
    cout << "|2. Customer       |\n";
    cout << "|3. Keluar         |\n";
    cout << "====================\n";
    cout << "Masukkan pilihan:  \n";
    cin >> input;

    if (input == "1")
    {
        loginAdmin(rootKonsol, headUser, gameKonsol);
    }else if (input == "2")
    {
        customerMenu(rootKonsol);
    }else if (input == "3")
    {
        cout << "Terimakasih :)";
    } else
    {
        cout << "Pilihan tidak valid. Silakan pilih lagi.\n"; 
        main();
    }

    return 0;
}