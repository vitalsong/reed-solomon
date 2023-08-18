#include <reed-solomon.h>
#include <rslib.h>

#include <cstddef>
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <stdexcept>

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

}   // namespace

ReedSolomon::ReedSolomon(int msg, int ecc, int bitlen)
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

    rs_ = init_rs(nbit_, PRIM_POLY[nbit_], 0, 1, ecc_);
    if (rs_ == nullptr) {
        throw std::runtime_error("Init reed solomon structure error");
    }
}

ReedSolomon::~ReedSolomon()
{
    free_rs(rs_);
}

std::vector<uint8_t> ReedSolomon::encode(const std::vector<uint8_t>& message)
{
    if (message.size() != msg_) {
        throw std::runtime_error("not valid message size for encode");
    }

    std::vector<uint16_t> par(ecc_);
    std::vector<uint8_t> enc(msg_ + ecc_);
    std::memcpy(enc.data(), message.data(), msg_);
    encode_rs8(rs_, enc.data(), msg_, par.data(), 0);
    for (int i = 0; i < ecc_; i++) {
        enc[i + msg_] = par[i];
    }
    return enc;
}

std::vector<uint8_t> ReedSolomon::decode(const std::vector<uint8_t>& block, const std::vector<uint8_t>& erasures)
{
    if (block.size() != (msg_ + ecc_)) {
        throw std::runtime_error("not valid block size for decode");
    }

    if (erasures.size() > ecc_) {
        return std::vector<uint8_t>(block.begin(), block.begin() + msg_);
    }

    std::vector<uint16_t> par(ecc_);
    std::vector<uint8_t> dec(msg_);
    for (int i = 0; i < ecc_; i++) {
        par[i] = block[i + msg_];
    }

    std::vector<int> eras(erasures.begin(), erasures.end());
    std::memcpy(dec.data(), block.data(), msg_);
    std::vector<uint16_t> corr(msg_ + ecc_);
    int nerr = decode_rs8(rs_, dec.data(), par.data(), msg_, nullptr, eras.size(), eras.data(), 0, nullptr);
    if (nerr < 0) {
        nerr = msg_;
    }

    return dec;
}

std::vector<uint8_t> ReedSolomon::decode(const std::vector<uint8_t>& block)
{
    return decode(block, {});
}
