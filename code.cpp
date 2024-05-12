#include <iostream>
#include <cstdint>
#include <vector>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

// Define a basic block structure
struct Block {
    uint32_t index;
    int64_t timestamp;
    std::string data;
    std::string previousHash;
    std::string hash;

    Block(uint32_t index, const std::string& data)
        : index(index), data(data) {
        timestamp = std::time(nullptr);
        previousHash = "";
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << timestamp << data << previousHash;
        std::string input = ss.str();

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input.c_str(), input.length());
        SHA256_Final(hash, &sha256);

        std::stringstream hashStream;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hashStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return hashStream.str();
    }
};

// Define a basic blockchain class
class Blockchain {
private:
    std::vector<Block> chain;

public:
    Blockchain() {
        chain.emplace_back(Block(0, "Genesis Block"));
    }

    void addBlock(const std::string& data) {
        Block newBlock(chain.size(), data);
        newBlock.previousHash = chain.back().hash;
        newBlock.hash = newBlock.calculateHash();
        chain.push_back(newBlock);
    }

    void printChain() const {
        for (const auto& block : chain) {
            std::cout << "Block #" << block.index << std::endl;
            std::cout << "Timestamp: " << block.timestamp << std::endl;
            std::cout << "Data: " << block.data << std::endl;
            std::cout << "Previous Hash: " << block.previousHash << std::endl;
            std::cout << "Hash: " << block.hash << std::endl;
            std::cout << std::endl;
        }
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            const Block& currentBlock = chain[i];
            const Block& previousBlock = chain[i - 1];

            if (currentBlock.hash != currentBlock.calculateHash()) {
                return false;
            }

            if (currentBlock.previousHash != previousBlock.hash) {
                return false;
            }
        }
        return true;
    }
};

int main() {
    Blockchain blockchain;

    std::cout << "Adding block 1..." << std::endl;
    blockchain.addBlock("Block 1 Data");
    std::cout << "Adding block 2..." << std::endl;
    blockchain.addBlock("Block 2 Data");

    std::cout << "Blockchain integrity: " << (blockchain.isChainValid() ? "Valid" : "Invalid") << std::endl;

    std::cout << "Blockchain:" << std::endl;
    blockchain.printChain();

    return 0;
}
