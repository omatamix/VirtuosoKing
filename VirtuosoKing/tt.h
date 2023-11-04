#pragma once
#include <cassert>
#include <cstdint>
#include <vector>
enum ScoreBound {
    EXACT,      // The score is exact.
    UPPERBOUND, // The score is an upper bound.
    LOWERBOUND, // The score is a lower bound.
};
struct HashTableEntry {
    uint64_t key = 0;         // The hash key corresponding to a board position.
    int depth = 0;            // Depth of the search that resulted in this entry.
    uint64_t move = 0;        // The move associated with this entry.
    int score = 0;            // The score of the position.
    int eval = -321114;       // Evaluation of the position without search.
    ScoreBound bound = EXACT; // The type of score bound.
    int isPv = 0;             // Indicates if this entry is on the principal variation.
    unsigned generation = 0;  // Generation or timestamp for entry freshness.
};
class TranspositionTable {
public:
    explicit TranspositionTable(size_t table_size)
        : table_size_(table_size), hash_table_(table_size), currentGeneration(0) {
        assert(table_size_ && !(table_size_ & (table_size_ - 1)) && "table_size_ should be a power of two");
        assert(table_size_ > 0 && "transposition table_size = 0");
    }
    TranspositionTable(const TranspositionTable&) = delete;
    TranspositionTable& operator=(const TranspositionTable&) = delete;
    TranspositionTable(TranspositionTable&&) noexcept = default;
    TranspositionTable& operator=(TranspositionTable&&) noexcept = default;
    const HashTableEntry* get(uint64_t key) const;
    void save(uint64_t key, int depth, uint64_t move, int score, int eval, ScoreBound bound, int isPv);
    void clear();
    void newSearch();
    ~TranspositionTable() = default;
private:
    size_t table_size_;
    std::vector<HashTableEntry> hash_table_;
    unsigned currentGeneration;
};
