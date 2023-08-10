#pragma once
#include <cstdint>
#include "search.h"
typedef uint64_t Key;
enum ScoreBound {
    EXACT = 0, LOWER_BOUND = 1, UPPER_BOUND = 2,
};
struct HashTableEntry {
    int64_t key;
    int depth;
    std::string move;
    int score;
    ScoreBound bound;
    bool isPv;
};
class TranspositionTable {
public:
    TranspositionTable(size_t table_size);
    const HashTableEntry* get(int64_t key);
    void save(int64_t key, int depth, std::string move,
        int score, ScoreBound bound, bool isPv);
    ~TranspositionTable() {
        if (hash_table_ != nullptr) {
            free(hash_table_);
        }
    }
private:
    HashTableEntry* hash_table_ = nullptr;
    size_t table_size_ = 0;
};