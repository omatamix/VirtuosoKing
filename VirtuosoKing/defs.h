#pragma once
#pragma warning(disable : 26812)
#include <chrono>
#include <string>
typedef int Depth;
typedef std::chrono::high_resolution_clock ChessClock;
typedef std::chrono::high_resolution_clock::time_point ChessTime;
typedef int Score;
typedef int PosKey;
constexpr char PIECE_LETTERS[7] = {
    ' ', 'P', 'N', 'B', 'R', 'Q', 'K'
};
constexpr char COLORS[5] = {
    ' ', 'r', 'b', 'y', 'g'
};
constexpr int BASE_MAILBOX[224] = {
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
};
constexpr int PIECE_ZERO = 0;
constexpr int MATE_SCORE = 99999;
constexpr char LETTER_COORDINATES[224] = {
    '?', '?', '?', '?', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', '?', '?', '?', '?',
    '?', '?', '?', '?', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', '?', '?', '?', '?',
    '?', '?', '?', '?', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', '?', '?', '?', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', '?',
    '?', '?', '?', '?', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', '?', '?', '?', '?',
    '?', '?', '?', '?', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', '?', '?', '?', '?',
    '?', '?', '?', '?', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', '?', '?', '?', '?'
};
const std::string NUMBER_COORDINATES[224] = {
    "?", "?",  "?",  "?",  "14", "14", "14", "14", "14", "14", "14", "14", "?",  "?",  "?",  "?",
    "?", "?",  "?",  "?",  "13", "13", "13", "13", "13", "13", "13", "13", "?",  "?",  "?",  "?",
    "?", "?",  "?",  "?",  "12", "12", "12", "12", "12", "12", "12", "12", "?",  "?",  "?",  "?",
    "?", "11", "11", "11", "11", "11", "11", "11", "11", "11", "11", "11", "11", "11", "11", "?",
    "?", "10", "10", "10", "10", "10", "10", "10", "10", "10", "10", "10", "10", "10", "10", "?",
    "?", "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "9",  "?",
    "?", "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "8",  "?",
    "?", "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "7",  "?",
    "?", "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "6",  "?",
    "?", "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "5",  "?",
    "?", "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "4",  "?",
    "?", "?",  "?",  "?",  "3",  "3",  "3",  "3",  "3",  "3",  "3",  "3",  "?",  "?",  "?",  "?",
    "?", "?",  "?",  "?",  "2",  "2",  "2",  "2",  "2",  "2",  "2",  "2",  "?",  "?",  "?",  "?",
    "?", "?",  "?",  "?",  "1",  "1",  "1",  "1",  "1",  "1",  "1",  "1",  "?",  "?",  "?",  "?"
};
constexpr int OFFSETS_NUM[11] = { 0, 3, 3, 3, 3, 8, 4, 4, 8, 8, 16, };
constexpr int OFFSETS[11][16] = {{},
    { -16, -17, -15,  0,  0,  0,  0,   0,  0,   0,   0,   0,  0,  0,  0,  0, },
    {  1,   17, -15,  0,  0,  0,  0,   0,  0,   0,   0,   0,  0,  0,  0,  0, },
    {  16,  17,  15,  0,  0,  0,  0,   0,  0,   0,   0,   0,  0,  0,  0,  0, },
    { -1,  -17,  15,  0,  0,  0,  0,   0,  0,   0,   0,   0,  0,  0,  0,  0, },
    { -18, -33, -31, -14, 18, 33, 31,  14, 0,   0,   0,   0,  0,  0,  0,  0, },
    { -17, -15,  17,  15, 0,  0,  0,   0,  0,   0,   0,   0,  0,  0,  0,  0, },
    { -16,  1,   16, -1,  0,  0,  0,   0,  0,   0,   0,   0,  0,  0,  0,  0, },
    { -17, -16, -15,  1,  17, 16, 15, -1,  0,   0,   0,   0,  0,  0,  0,  0, },
    { -17, -16, -15,  1,  17, 16, 15, -1,  0,   0,   0,   0,  0,  0,  0,  0, },
    { -17, -16, -15,  1,  17, 16, 15, -1, -18, -33, -31, -14, 18, 33, 31, 14 },
};
constexpr int CASTLING_CK_SQRSK[5][2] = { {},
    { 214, 215 }, { 126, 142 },
    { 3,   2   }, { 91,  75  },
};
constexpr int CASTLING_CK_SQRSQ[5][2] = { {},
    { 212, 211 }, { 94,  78  },
    { 5,   6   }, { 123, 139 },
};
constexpr int OPPOSITES[5][2] = { {},
    { 2, 4, }, { 1, 3, },
    { 2, 4, }, { 1, 3, },
};
constexpr int CST_ROOK_LOCATIONS[2][5] = {
    { 0, 216, 158, 1, 59  },
    { 0, 209, 46,  8, 171 },
};
constexpr int PARTNERS[5] = { 0, 3, 4, 1, 2 };
constexpr int DEFAULT_NUM_THREADS = 1;
constexpr int MAX_NUM_THREADS = 128;
constexpr int MAX_SEARCH_DEPTH = 99;
constexpr int MAX_SEARCH_PLY = 70;
constexpr int MAX_MOVES = 256;
constexpr int MAX_PLY_MATE_SCORE = 32500;
constexpr int INFTY = 32767;
constexpr int MAX_REDUCTION_ALLOWED = 25;
constexpr int NEAR_MATE_SCORE = 2500;
constexpr int ONE_PLY = 1;
constexpr int TIME = 1;
constexpr int MOVETIME = 2;
constexpr int DEPTH = 4;
constexpr int ASPIRATION_MARGIN = 50;
constexpr int MAX_EXTENSION_MOVE_COUNT = 7;
constexpr Depth DEPTH_ZERO = 0;
constexpr uint64_t ONE_SECOND = 1000;
constexpr uint64_t MAX_TIME = (1ULL << 63) - 1;
constexpr int TT_MAX_SIZE = 10000000;
constexpr int FUTILITY_MARGIN = 167;