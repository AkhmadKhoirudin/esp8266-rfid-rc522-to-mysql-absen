<?php
// Konfigurasi database
$host = 'localhost';
$dbname = 'absen'; // Ganti dengan nama database Anda
$user = 'root'; // Ganti dengan nama pengguna database Anda
$pass = ''; // Ganti dengan kata sandi database Anda

// Membuat koneksi ke database
try {
    $conn = new PDO("mysql:host=$host;dbname=$dbname", $user, $pass);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
    die("Koneksi database gagal: " . $e->getMessage());
}

// Memeriksa apakah UID diterima melalui POST
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_POST['uid'])) {
    // Menerima UID
    $uid = $_POST['uid'];

    // Mendapatkan jam dan tanggal saat ini
    date_default_timezone_set('Asia/Jakarta'); // Atur zona waktu yang diinginkan
    $tanggal = date('Y-m-d');
    $jam = date('H:i:s');

    // Mencari nama pegawai berdasarkan UID dari tabel pegawai
    $stmt_name = $conn->prepare("SELECT name FROM pegawai WHERE uid = :uid");
    $stmt_name->bindParam(':uid', $uid);
    $stmt_name->execute();
    $result = $stmt_name->fetch(PDO::FETCH_ASSOC);

    if ($result) {
        $name = $result['name'];

        // Pastikan name tidak null atau kosong
        if ($name !== null && $name !== '') {
            // Periksa apakah sudah ada entri dengan UID dan tanggal yang sama dalam tabel kartu
            $stmt_check = $conn->prepare("SELECT * FROM kartu WHERE uid = :uid AND tanggal = :tanggal");
            $stmt_check->bindParam(':uid', $uid);
            $stmt_check->bindParam(':tanggal', $tanggal);
            $stmt_check->execute();
            $existing_entry = $stmt_check->fetch(PDO::FETCH_ASSOC);

            if ($existing_entry) {
                echo "Data sudah ada di database untuk tanggal ini. Data tidak disimpan lagi.";
            } else {
                // Menyimpan data ke database jika belum ada entri dengan UID dan tanggal yang sama
                $stmt = $conn->prepare("INSERT INTO kartu (uid, name, tanggal, jam) VALUES (:uid, :name, :tanggal, :jam)");
                $stmt->bindParam(':uid', $uid);
                $stmt->bindParam(':name', $name);
                $stmt->bindParam(':tanggal', $tanggal);
                $stmt->bindParam(':jam', $jam);

                if ($stmt->execute()) {
                    echo "Data berhasil disimpan ke database.";
                } else {
                    echo "Error menyimpan data ke database.";
                }
            }
        } else {
            echo "Nama pegawai tidak ditemukan atau kosong. Data tidak disimpan ke tabel kartu.";
        }
    } else {
        // Jika UID tidak ditemukan di tabel pegawai, simpan UID ke tabel pegawai
        // Anda mungkin ingin meminta informasi nama sebelum menyimpan data ke tabel pegawai
        // Misalnya, bisa menggunakan input form tambahan

        // Ini contoh yang sederhana: hanya menyimpan UID ke tabel pegawai tanpa nama
        $stmt_pegawai = $conn->prepare("INSERT INTO pegawai (uid) VALUES (:uid)");
        $stmt_pegawai->bindParam(':uid', $uid);

        if ($stmt_pegawai->execute()) {
            echo "UID tidak ditemukan di tabel pegawai, UID disimpan ke tabel pegawai.";
        } else {
            echo "Error menyimpan UID ke tabel pegawai.";
        }
    }
}
?>
