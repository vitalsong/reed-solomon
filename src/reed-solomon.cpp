#include <reed-solomon.h>
#include <rslib.h>

#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <stdexcept>
#include <cstring>
#include <map>

namespace {

const uint16_t PRIM_POLY[9] = {
  0,      ///< not valid
  0,      ///< not valid
  0x7,    ///< 2^2
  0xb,    ///< 2^3
  0x13,   ///< 2^4
  0x2f,   ///< 2^5
  0x6d,   ///< 2^6
  0xb9,   ///< 2^7
  0x187   ///< 2^8
};

struct RsParamKey
{
    explicit RsParamKey(uint8_t msg, uint8_t ecc, uint8_t nbit)
    {
        _hash = (uint32_t(msg) << 16) | (uint32_t(ecc) << 8) | nbit;
    }

    bool operator<(const RsParamKey& rhs) const noexcept
    {
        return _hash < rhs._hash;
    }

    bool operator==(const RsParamKey& rhs) const noexcept
    {
        return _hash == rhs._hash;
    }

private:
    uint32_t _hash{0};
};

std::shared_ptr<rs_control> _get_rs(uint8_t msg_len, uint8_t ecc_len, uint8_t bit_len)
{
    thread_local std::map<RsParamKey, std::weak_ptr<rs_control>> cache;
    const RsParamKey key{msg_len, ecc_len, bit_len};
    if (cache.count(key) == 0 || cache.at(key).expired()) {
        auto ptr = init_rs(bit_len, PRIM_POLY[bit_len], 0, 1, ecc_len);
        if (ptr == nullptr) {
            throw std::runtime_error("Init reed solomon structure error");
        }
        auto rs = std::shared_ptr<rs_control>(ptr, free_rs);
        cache[key] = std::weak_ptr<rs_control>(rs);
        return rs;
    }
    return cache[key].lock();
}

}   // namespace

ReedSolomon::ReedSolomon(uint8_t msg, uint8_t ecc, uint8_t bitlen)
  : msg_(msg)
  , ecc_(ecc)
  , nbit_(bitlen)
{
    if ((nbit_ <= 1) || (nbit_ > 8)) {
        throw std::runtime_error("bitlen must be from 2 to 8");
    }

    if ((ecc_ + msg_) >= (1 << nbit_)) {
        throw std::runtime_error("block size cannot be greater than 2^nbit-1");
    }

    rs_ = _get_rs(msg, ecc, bitlen);
}

std::vector<uint8_t> ReedSolomon::encode(const std::vector<uint8_t>& message)
{
    if (message.size() != msg_) {
        throw std::runtime_error("not valid message size for encode");
    }

    std::vector<uint16_t> par(ecc_);
    std::vector<uint8_t> enc(msg_ + ecc_);
    std::memcpy(enc.data(), message.data(), msg_);
    encode_rs8(rs_.get(), enc.data(), msg_, par.data(), 0);
    for (int i = 0; i < ecc_; i++) {
        enc[i + msg_] = par[i];
    }
    return enc;
}

std::pair<std::vector<uint8_t>, int> ReedSolomon::decode(const std::vector<uint8_t>& block,
                                                         const std::vector<uint8_t>& erasures)
{
    if (block.size() != (msg_ + ecc_)) {
        throw std::runtime_error("not valid block size for decode");
    }

    if (erasures.size() > ecc_) {
        return decode(block);
    }

    std::vector<uint16_t> par(ecc_);
    std::vector<uint8_t> dec(msg_);
    for (int i = 0; i < ecc_; i++) {
        par[i] = block[i + msg_];
    }

    std::vector<int> eras(erasures.begin(), erasures.end());
    std::memcpy(dec.data(), block.data(), msg_);
    std::vector<uint16_t> corr(msg_ + ecc_);
    int nerr = decode_rs8(rs_.get(), dec.data(), par.data(), msg_, nullptr, eras.size(), eras.data(), 0, nullptr);
    if (nerr < 0) {
        nerr = msg_;
    }

    return std::make_pair(dec, nerr);
}

std::pair<std::vector<uint8_t>, int> ReedSolomon::decode(const std::vector<uint8_t>& block)
{
    return decode(block, {});
}
