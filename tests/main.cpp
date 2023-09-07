#include <cassert>
#include <cstdint>
#include <cstring>

#include <gtest/gtest.h>
#include <reed-solomon.h>

using namespace testing;

class ReedSolomonTest
  : public Test
  , public WithParamInterface<int>
{};

INSTANTIATE_TEST_SUITE_P(RS, ReedSolomonTest, Values(3, 4, 5, 6, 7, 8));

TEST_P(ReedSolomonTest, BaseDecoder)
{
    const int nbit = GetParam();
    const int max_block = (1 << nbit) - 1;
    const int ecc = (max_block / 4) + 1;
    const int msg = max_block - ecc;

    //move test
    ReedSolomon rs{ReedSolomon(msg, ecc, nbit)};

    std::vector<uint8_t> message(msg);
    for (int i = 0; i < msg; ++i) {
        message[i] = rand() % (1 << nbit);
    }

    auto encoded = rs.encode(message);
    ASSERT_EQ(encoded.size(), msg + ecc);

    std::set<uint8_t> erasures;
    //guaranteed to fix n/2 errors
    for (int i = 0, pos = 0; i < (ecc / 2); i++) {
        while (erasures.count(pos) != 0) {
            pos = rand() % encoded.size();
        }
        erasures.insert(pos);
        encoded[pos] = 0;
    }

    auto [decoded, rs_err] = rs.decode(encoded);
    ASSERT_EQ(decoded.size(), msg);

    ASSERT_EQ(std::memcmp(message.data(), decoded.data(), msg), 0);
}

TEST_P(ReedSolomonTest, ErasuresDecoder)
{
    const int nbit = GetParam();
    const int max_block = (1 << nbit) - 1;
    const int ecc = (max_block / 4) + 1;
    const int msg = max_block - ecc;

    auto rs = ReedSolomon(msg, ecc, nbit);

    std::vector<uint8_t> message(msg);
    for (int i = 0; i < msg; ++i) {
        message[i] = rand() % (1 << nbit);
    }

    auto encoded = rs.encode(message);
    ASSERT_EQ(encoded.size(), msg + ecc);

    std::set<uint8_t> erasures;
    //guaranteed to fix ecc located errors
    for (int i = 0, pos = 0; i < ecc; i++) {
        while (erasures.count(pos) != 0) {
            pos = rand() % encoded.size();
        }
        erasures.insert(pos);
        encoded[pos] = 0;
    }

    auto [decoded, rs_err] = rs.decode(encoded, std::vector<uint8_t>(erasures.begin(), erasures.end()));
    ASSERT_EQ(decoded.size(), msg);

    ASSERT_EQ(std::memcmp(message.data(), decoded.data(), msg), 0);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
