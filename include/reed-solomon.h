#pragma once

#include <vector>
#include <cstdint>
#include <memory>

struct rs_control;

class ReedSolomon
{
public:
    explicit ReedSolomon(uint8_t msg, uint8_t ecc, uint8_t bitlen = 8);

    ReedSolomon(ReedSolomon&& rhs)
      : msg_{rhs.msg_}
      , ecc_{rhs.ecc_}
      , nbit_{rhs.nbit_}
      , rs_{rhs.rs_}
    {}

    ReedSolomon(const ReedSolomon&) = delete;

    std::vector<uint8_t> encode(const std::vector<uint8_t>& message);

    std::vector<uint8_t> decode(const std::vector<uint8_t>& block);

    std::vector<uint8_t> decode(const std::vector<uint8_t>& block, const std::vector<uint8_t>& erasures);

    int msg_len() const noexcept
    {
        return msg_;
    }

    int ecc_len() const noexcept
    {
        return ecc_;
    }

    int bitlen() const noexcept
    {
        return nbit_;
    }

private:
    uint8_t msg_;
    uint8_t ecc_;
    uint8_t nbit_;
    std::shared_ptr<rs_control> rs_;
};
