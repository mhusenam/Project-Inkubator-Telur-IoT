#  Project Inkubator Telur Berbasis IoT

Sistem otomatisasi dan pemantauan suhu serta kelembaban untuk inkubator telur berbasis Internet of Things (IoT). Project ini dirancang untuk menjaga kestabilan kondisi di dalam inkubator guna mengoptimalkan tingkat keberhasilan penetasan telur, lengkap dengan fitur kontrol dan notifikasi secara *real-time* melalui Telegram.

---

##  Fitur Utama

- **Monitoring Real-Time**: Pemantauan suhu dan kelembaban udara di dalam inkubator secara langsung.
- **Notifikasi Bot Telegram**: Mengirimkan informasi status sistem, peringatan suhu terlalu tinggi/rendah, dan laporan operasional secara otomatis ke Telegram.
- **Kontrol Sistem Dual Mode**: Mendukung mode **Otomatis** maupun **Manual** untuk pengaturan lampu pemanas, kipas, dan penggerak telur (servo).
- **Simulasi & Dokumentasi Lengkap**: Dilengkapi dengan skema perancangan jalur kabel (*wiring*) dan video simulasi unjuk kerja alat.

---

##  Struktur Direktori Project

```text
Project Inkubator Telur Berbasis IOT/
│
├── kode.cpp                  # Kode program utama mikrokontroler (C++)
├── tampilan telegram.jpeg    # Dokumentasi tampilan antarmuka bot Telegram
├── tampilan wiring.jpeg      # Dokumentasi fisik rangkaian/wiring perangkat
├── wiring pada aplikasi.jpeg   # Skema perancangan jalur kabel (diagram/aplikasi)
└── VIDEO SIMULASI .mp4       # Video demonstrasi cara kerja sistem inkubator
