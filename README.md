### C++ Reed-Solomon (based on Linux kernel implementation)

![testing](https://github.com/vitalsong/reed-solomon/actions/workflows/android-ndk.yml/badge.svg)
![testing](https://github.com/vitalsong/reed-solomon/actions/workflows/linux.yml/badge.svg)
![testing](https://github.com/vitalsong/reed-solomon/actions/workflows/windows.yml/badge.svg)

> For more detailed customization, you can use the original `rslib`

### Example

```cpp
int msg_len = 239;
int ecc_len = 16;
auto rs = ReedSolomon(msg_len, ecc_len, 8);
auto message = std::vector<uint8_t>(msg_len);
for (int i=0; i < message.size(); ++i) {
    message[i] = rand() % 256;
}

//encoder
auto encoded = rs.encode(message);
assert(encoded.size() == msg_len + ecc_len);

//add ecc_len/2 erasures
for (int i=0; i < rs.ecc_len()/2; ++i) {
    auto idx = rand() % encoded.size();
    encoded[idx] = 0;
}

//decoder
auto decoded = rs.decode(encoded);
// if you know the erasures positions a priori, you can recover ecc_len symbols
// auto decoded = rs.decode(encoded, erasures);
assert(decoded.size() == msg_len);

//data is equal
assert(std::strncmp((char*)message.data(), (char*)decoded.data(), msg_len) == 0);
```

### Build

```sh
cmake . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel=4
```