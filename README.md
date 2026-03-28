# Tucil 2 Strategi Algoritma (IF2211) - Voxelization 3D

## A. Deskripsi Program
Program ini mengonversi model 3D berformat `.obj` menjadi representasi *voxel* menggunakan paradigma *divide and conquer*. Algoritma mempartisi ruang secara spasial menggunakan *pointerless octree* — di mana setiap node Box dibagi menjadi 8 oktan secara rekursif tanpa struktur pointer eksplisit — dan mengekstraksi voxel pada kedalaman maksimum (`max_depth`).

Optimasi interseksi dilakukan dua tahap:
1. **AABB Fast Reject** — menolak face yang bounding box-nya tidak overlap dengan box saat ini di sumbu X, Y, Z.
2. **Plane Test** — mengecek apakah seluruh 8 corner box berada di satu sisi bidang segitiga (*separating axis theorem* berbasis dot product normal).

Keluaran program adalah file `.obj` baru berisi representasi *mesh* kubus seragam (*quad faces*, 6 face per voxel) pada permukaan objek, disimpan di direktori yang sama dengan file masukan.

## B. Requirement Program
Program murni menggunakan pustaka standar C++ (STL) tanpa *dependency* eksternal.
* Kompiler C++ dengan dukungan **C++17** (misal: `g++` ≥ 7 atau `clang++` ≥ 5).
* Sistem operasi: Linux (Arch/Ubuntu) atau Windows.

## C. Cara Kompilasi
Pastikan struktur *repository* sudah sesuai — source code berada di folder `src/`. Jalankan perintah berikut dari *root directory*:

```bash

g++ -O3 -std=c++17 src/converter.cpp -o bin/converter
```

Flag `-O3` sangat disarankan karena algoritma rekursif octree sensitif terhadap performa.

## D. Cara Menjalankan
Program menerima dua argumen: path file masukan `.obj` dan batas kedalaman maksimum octree.

```bash
./bin/converter <path_file_obj> <max_depth>
```

Contoh eksekusi:

```bash
./bin/converter test/pumpkin.obj 6
```

Setelah eksekusi selesai, terminal menampilkan report komputasi:

```
====================================
voxel : 12480
vertex: 99840
faces : 74880
depth : 6
time  : 0.843 s
output: test/pumpkin-voxelized.obj
------------------------------------
node terbentuk:
1: 1
2: 8
...
------------------------------------
node tidak ditelusuri:
1: 0
2: 3
...
====================================
```

File hasil konversi `<nama_input>-voxelized.obj` akan tersimpan otomatis di direktori yang sama dengan file masukan.

## E. Struktur Repository
```
tucil2_13524115/
├── bin/              # hasil kompilasi (tidak di-commit)
├── src/
│   └── converter.cpp # source utama
├── test/             # file .obj untuk pengujian
└── README.md
```

## F. Author
* **Nama:** Ega Luthfi Rais
* **NIM:** 13524115