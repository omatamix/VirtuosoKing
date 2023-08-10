#include <cassert>
#include <corecrt_malloc.h>
#include <cstring>
#include "tt.h"
TranspositionTable::TranspositionTable(size_t table_size) {
    assert((table_size > 0) && "transposition table_size = 0");
    table_size_ = table_size;
    hash_table_ = (HashTableEntry*)calloc(table_size, sizeof(HashTableEntry));
    assert(
        (hash_table_ != nullptr) &&
        "Can't create transposition table. Try using a smaller size.");
}
const HashTableEntry* TranspositionTable::get(int64_t key) {
    size_t n = key % table_size_;
    HashTableEntry* entry = hash_table_ + n;
    if (entry->key == key) {
        return entry;
    }
    return nullptr;
}
void TranspositionTable::save(
    int64_t key, int depth, std::string move, int score,
    ScoreBound bound, bool isPv) {
    size_t n = key % table_size_;
    HashTableEntry& entry = hash_table_[n];
    entry.key = key;
    entry.depth = depth;
    entry.move = move;
    entry.score = score;
    entry.bound = bound;
    entry.isPv = isPv;
}