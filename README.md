

# My Archiver

**My Archiver** là một công cụ dòng lệnh để **nén**, **giải nén**, **đóng gói** và **giải gói** tệp tin, hỗ trợ các định dạng **ZIP**, **TAR** và các thuật toán **Huffman**, **RLE**.

---

## 🚀 Features

- ✅ Nén và giải nén tệp tin sử dụng thuật toán **Huffman** và **RLE**
- 📦 Đóng gói và giải gói tệp tin vào/ra định dạng **TAR**
- 🗜️ Hỗ trợ định dạng **ZIP** (hiện tại chỉ ở chế độ lưu trữ không nén)
- 📝 Ghi log hoạt động vào file và console với tùy chọn `-v` (verbose)

---

## 📚 Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Available Commands](#available-commands)
- [Examples](#examples)
- [Limitations](#limitations)
- [Contributing](#contributing)
- [License](#license)

---

## ⚙️ Installation

### Prerequisites

Bạn cần cài đặt:
- `GCC` (GNU Compiler Collection)
- `make`

### On Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential
```

### On macOS
```bash
brew install gcc
```

### On Windows
- Sử dụng [MinGW](https://sourceforge.net/projects/mingw/) để cài đặt GCC
- Hoặc dùng [WSL](https://learn.microsoft.com/en-us/windows/wsl/) để có môi trường Linux

### Clone the Repository
```bash
git clone https://github.com/trijaze/comp-tool.git
cd comp-tool
```

### Build the Tool
```bash
make
```

File thực thi `comp-tool` sẽ được tạo trong thư mục hiện tại.

### Clean Build Files (optional)
```bash
make clean
```

---

## 🧰 Usage

### General Syntax
```bash
./comp-tool [-v] <operation> <format> <input> <output>
```

- `operation`: Hành động muốn thực hiện (`compress`, `decompress`, `pack`, `unpack`)
- `format`: Thuật toán hoặc định dạng (`zip`, `tar`, `huffman`, `rle`)
- `input`: Đường dẫn đến tệp đầu vào
- `output`: Đường dẫn đến tệp hoặc thư mục đầu ra
- `-v`: (tùy chọn) In thông tin chi tiết quá trình thực thi

---

## 📄 Available Commands

| Command    | Description                                         |
|------------|-----------------------------------------------------|
| `compress` | Nén tệp sử dụng `zip`, `huffman`, hoặc `rle`        |
| `decompress` | Giải nén tệp từ `zip`, `huffman`, hoặc `rle`     |
| `pack`     | Đóng gói tệp vào định dạng `TAR`                    |
| `unpack`   | Giải gói tệp từ định dạng `TAR`                     |
| `-v`       | Kích hoạt chế độ verbose để in chi tiết             |

---

## 🧪 Examples

### Nén tệp với Huffman
```bash
./comp-tool -v compress huffman input.txt output.huff
```

### Giải nén Huffman
```bash
./comp-tool decompress huffman output.huff output.txt
```

### Nén tệp với RLE
```bash
./comp-tool -v compress rle input.txt output.rle
```

### Giải nén RLE
```bash
./comp-tool decompress rle output.rle output.txt
```

### Nén với định dạng ZIP (stored mode)
```bash
./comp-tool -v compress zip input.txt output.zip
```

### Giải nén ZIP
```bash
./comp-tool decompress zip output.zip output.txt
```

### Đóng gói tệp vào TAR
```bash
./comp-tool -v pack tar input.txt output.tar
```

### Giải gói từ TAR
```bash
./comp-tool unpack tar output.tar output_dir
```

---

## ⚠️ Limitations

- ❗ Chỉ hỗ trợ xử lý **một tệp duy nhất** cho mỗi lần chạy
- 🔒 ZIP chỉ hỗ trợ **lưu trữ (stored method)**, không nén thực sự
- 🧠 Huffman & RLE đọc toàn bộ tệp vào RAM ⇒ **chưa tối ưu với tệp lớn**
- 🚫 Chưa hỗ trợ lệnh `list` để xem nội dung trong archive
- 📁 Không hỗ trợ nén cả thư mục hoặc sử dụng mật khẩu

---
