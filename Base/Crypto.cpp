#include "Crypto.h"

#include <memory>

#include <openssl/evp.h>
#include <openssl/sha.h>

std::vector<uint8_t> BS::Crypto::compute_sha_256(std::span<const uint8_t> bytes) {

    std::vector<uint8_t> result(SHA256_DIGEST_LENGTH);

    SHA256(bytes.data(), bytes.size(), result.data());

    return result;
}

std::vector<uint8_t> BS::Crypto::compute_sha_256(std::istream &input, const std::size_t buffer_size) {

    if (buffer_size < 1)
        throw std::invalid_argument("buffer_size can not be less than 1.");

    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> md_ctx(
            EVP_MD_CTX_new(),
            &EVP_MD_CTX_free);

    if (!EVP_DigestInit_ex2(md_ctx.get(), EVP_sha256(), nullptr))
        throw std::runtime_error("Message digest initialization failed.");

    std::vector<uint8_t> buffer(buffer_size);

    while (true) {

        input.read(
            reinterpret_cast<char *>(buffer.data()),
            static_cast<std::streamsize>(buffer.size()));

        const auto read = input.gcount();

        if (read <= 0)
            break;

        if (!EVP_DigestUpdate(md_ctx.get(), buffer.data(), read))
            throw std::runtime_error("Message digest update failed.");
    }

    if (input.fail() && !input.eof())
        throw std::runtime_error("Unable to compute SHA 256. An error occurred while reading data from stream.");

    std::vector<uint8_t> result(SHA256_DIGEST_LENGTH);

    if (!EVP_DigestFinal_ex(md_ctx.get(), result.data(), nullptr))
        throw std::runtime_error("Message digest finalization failed.");

    return result;
}
