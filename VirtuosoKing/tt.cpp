#include "tt.h"
const HashTableEntry* TranspositionTable::get(uint64_t key) const {
    size_t n = key & (table_size_ - 1);
    const HashTableEntry& entry = hash_table_[n];
    if (entry.key == key && entry.generation == currentGeneration) {
        return &entry;
    }
    return nullptr;
}
void TranspositionTable::save(uint64_t key, int depth, uint64_t move, int score, int eval,
    ScoreBound bound, int isPv) {
    size_t n = key & (table_size_ - 1);
    HashTableEntry& entry = hash_table_[n];
    if (entry.key == 0 || entry.generation < currentGeneration ||
       (entry.generation == currentGeneration && entry.depth < depth)) {
        used_entries_++;
        entry.key = key;
        entry.depth = depth;
        entry.move = move;
        entry.score = score;
        entry.eval = eval;
        entry.bound = bound;
        entry.isPv = isPv;
        entry.generation = currentGeneration;
    }
}
int TranspositionTable::hashfull() const {
    return static_cast<int>(std::round((static_cast<double>(used_entries_) /
           static_cast<double>(table_size_)) * 1000.0));
}
void TranspositionTable::clear() {
    std::fill(hash_table_.begin(), hash_table_.end(), HashTableEntry{});
    used_entries_ = 0;
}
void TranspositionTable::newSearch() {
    currentGeneration++;
}



