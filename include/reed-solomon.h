#pragma once

#include <vector>
#include <cstdint>

struct rs_control;

class ReedSolomon
{
public:
    explicit ReedSolomon(int msg, int ecc, int bitlen = 8);
    ~ReedSolomon();

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
    int msg_;
    int ecc_;
    int nbit_;
    rs_control* rs_{nullptr};
};
