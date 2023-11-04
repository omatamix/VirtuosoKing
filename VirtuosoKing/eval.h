#pragma once
#include "defs.h"
constexpr int SQUARE_CNT = 224;
constexpr int MATERIAL_SCORES_MG[7] = { 0, 55,  295, 375, 525, 1345, MATE_SCORE };
constexpr int MATERIAL_SCORES_EG[7] = { 0, 110, 295, 350, 500, 1575, MATE_SCORE };
constexpr int MOBILITY_SCORES[7] = {
    0, 0, 1, 2, 3, 4, 0
};
constexpr int MOBILITY_SCORES_NEW[7] = {
    0, 0, 1, 2, 3, 4, 0
};
constexpr int NO_PIECES_LEFT_PENALTY = -675;
constexpr int ONE_PIECE_LEFT_PENALTY = -225;
constexpr int LAZY_EVAL_THRESHOLD_MG = 675;
constexpr int LAZY_EVAL_THRESHOLD_EG = 1655;
constexpr int KNIGHT_BACKRANK_PST[SQUARE_CNT] = {
    -1, -1, -1, -1, -10,-10,-10,-10,-10,-10,-10,-10,-1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1, -10, 0,  0,  0,  0,  0,  0, -10,-1, -1, -1, -1,
    -1, -10, 0, -10, 0,  0,  0,  0,  0,  0,  0,  0, -10, 0, -10,-1,
    -1, -10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -10,-1,
    -1, -10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -10,-1,
    -1, -10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -10,-1,
    -1, -10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -10,-1,
    -1, -10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -10,-1,
    -1, -10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -10,-1,
    -1, -10, 0, -10, 0,  0,  0,  0,  0,  0,  0,  0, -10, 0, -10,-1,
    -1, -1, -1, -1, -10, 0,  0,  0,  0,  0,  0, -10,-1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1, -10,-10,-10,-10,-10,-10,-10,-10,-1, -1, -1, -1,
};
constexpr int ENDGAME_CENTRAL_OR_CENTRAL_OPENING_MIDDLEGAME_QUEEN_PST[SQUARE_CNT] = {
    -1, -1, -1, -1, -3,  3,  3,  3,  3,  3,  3, -3, -1, -1, -1, -1,
    -1, -1, -1, -1,  3,  7,  7,  7,  7,  7,  7,  3, -1, -1, -1, -1,
    -1, -1, -1, -1,  7,  11, 11, 11, 11, 11, 11, 7, -1, -1, -1, -1,
    -1, -3,  3,  7,  11, 13, 13, 13, 13, 13, 13, 11, 7,  3,  0, -1,
    -1,  3,  7,  11, 13, 16, 16, 16, 16, 16, 16, 13, 11, 7,  3, -1,
    -1,  3,  7,  11, 13, 16, 24, 24, 24, 24, 16, 13, 11, 7,  3, -1,
    -1,  3,  7,  11, 13, 16, 24, 37, 37, 24, 16, 13, 11, 7,  3, -1,
    -1,  3,  7,  11, 13, 16, 24, 37, 37, 24, 16, 13, 11, 7,  3, -1,
    -1,  3,  7,  11, 13, 16, 24, 24, 24, 24, 16, 13, 11, 7,  3, -1,
    -1,  3,  7,  11, 13, 16, 16, 16, 16, 16, 16, 13, 11, 7,  3, -1,
    -1, -3,  3,  7,  11, 13, 13, 13, 13, 13, 13, 11, 7,  3, -3, -1,
    -1, -1, -1, -1,  7,  11, 11, 11, 11, 11, 11, 7, -1, -1, -1, -1,
    -1, -1, -1, -1,  3,  7,  7,  7,  7,  7,  7,  3, -1, -1, -1, -1,
    -1, -1, -1, -1, -3,  3,  3,  3,  3,  3,  3, -3, -1, -1, -1, -1,
};
constexpr int ENDGAME_PAWN_PROGRESSION_PST[2][SQUARE_CNT] = { {
    -1, -1, -1, -1, -50, -50, -50, -50, -50, -50, -50, -50, -1, -1, -1, -1,
    -1, -1, -1, -1, -50, -50, -50, -50, -50, -50, -50, -50, -1, -1, -1, -1,
    -1, -1, -1, -1, -50, -50, -50, -50, -50, -50, -50, -50, -1, -1, -1, -1,
    -1, -45,-45,-45,-45, -45, -45, -45, -45, -45, -45, -45, -45,-45,-45,-1,
    -1, -25,-25,-25,-25, -25, -25, -25, -25, -25, -25, -25, -25,-25,-25,-1,
    -1, -15,-15,-15,-15, -15, -15, -15, -15, -15, -15, -15, -15,-15,-15,-1,
    -1, -5, -5, -5, -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5, -5, -5, -1,
    -1, -5,  5,  5,  5,   5,   5,   5,   5,   5,   5,   5,   5,  5,  5, -1,
    -1,  15, 15, 15, 15,  15,  15,  15,  15,  15,  15,  15,  15, 15, 15,-1,
    -1,  25, 25, 25, 25,  25,  25,  25,  25,  25,  25,  25,  25, 25, 25,-1,
    -1,  45, 45, 45, 45,  45,  45,  45,  45,  45,  45,  45,  45, 45, 45,-1,
    -1, -1, -1, -1,  50,  50,  50,  50,  50,  50,  50,  50, -1, -1, -1, -1,
    -1, -1, -1, -1,  50,  50,  50,  50,  50,  50,  50,  50, -1, -1, -1, -1,
    -1, -1, -1, -1,  50,  50,  50,  50,  50,  50,  50,  50, -1, -1, -1, -1,
}, {
    -1, -1, -1, -1,  45, 25, 15, 5, -5, -15, -25, -45, -1, -1, -1, -1,
    -1, -1, -1, -1,  45, 25, 15, 5, -5, -15, -25, -45, -1, -1, -1, -1,
    -1, -1, -1, -1,  45, 25, 15, 5, -5, -15, -25, -45, -1, -1, -1, -1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1,  50, 50, 50, 45, 25, 15, 5, -5, -15, -25, -45, -50,-50,-50,-1,
    -1, -1, -1, -1,  45, 25, 15, 5, -5, -15, -25, -45, -1, -1, -1, -1,
    -1, -1, -1, -1,  45, 25, 15, 5, -5, -15, -25, -45, -1, -1, -1, -1,
    -1, -1, -1, -1,  45, 25, 15, 5, -5, -15, -25, -45, -1, -1, -1, -1,
} };
constexpr int PAWN_STRUCT_PST_TABLE_OPENING[2][SQUARE_CNT] = { {
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1, -22, 11,-39, 28,-34,-12, 17,-19,-1, -1, -1, -1,
    -1,  0,  0,  14, 15, 9, -49,-17, 30, 12, 20,-7,  17, 0,  0, -1,
    -1,  0,  7,  12, 17,-5, -66,-22,-31,-12,-12,-12, 12, 7,  0, -1,
    -1,  12, 7, -13, 6, -9, -33,-22,-22,-15,-15,-15, 13, 12, 7, -1,
    -1, -20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-1,
    -1, -20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-20,-1,
    -1,  7,  12, 13,-15,-15,-15,-22,-22,-33,-9,  6, -13, 7,  12,-1,
    -1,  0,  7,  12,-12,-12,-12,-31,-22,-66,-5,  17, 12, 7,  0, -1,
    -1,  0,  0,  17,-7,  20, 12, 30,-17,-49, 9,  15, 14, 0,  0, -1,
    -1, -1, -1, -1, -19, 17,-12,-34, 28,-39, 11,-22,-1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
}, {
    -1, -1, -1, -1,  0,  0,  7, -20,-20, 12, 0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  7,  12,-20,-20, 7,  7,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  17, 12, 13,-20,-20,-13, 12, 14,-1, -1, -1, -1,
    -1,  0,  0, -19,-7, -12,-15,-20,-20, 6,  17, 15,-22, 0,  0, -1,
    -1,  0,  0,  17, 20,-12,-15,-20,-20,-9, -5,  9,  11, 0,  0, -1,
    -1,  0,  0, -12, 12,-12,-15,-20,-20,-33,-66,-49,-39, 0,  0, -1,
    -1,  0,  0, -34, 30,-31,-22,-20,-20,-22,-22,-17, 28, 0,  0, -1,
    -1,  0,  0,  28,-17,-22,-22,-20,-20,-22,-31, 30,-34, 0,  0, -1,
    -1,  0,  0, -39,-49,-66,-33,-20,-20,-15,-12, 12,-12, 0,  0, -1,
    -1,  0,  0,  11, 9, -5, -9, -20,-20,-15,-12, 20, 17, 0,  0, -1,
    -1,  0,  0, -22, 15, 17, 6, -20,-20,-15,-12,-7, -19, 0,  0, -1,
    -1, -1, -1, -1,  14, 12,-13,-20,-20, 13, 12, 17,-1, -1, -1, -1,
    -1, -1, -1, -1,  0,  7,  7, -20,-20, 12, 7,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  12,-20,-20, 7,  0,  0, -1, -1, -1, -1,
} };

constexpr int UNDEVELOPED_KNIGHT_PENALTY_NEW = 23;
constexpr int UNDEVELOPED_KNIGHT_PENALTY = 37;
constexpr int UNDEVELOPED_QUEEN_PENALTY = 37;
constexpr int ATTACKED_KING_SQUARE_PENALTY = 13;
constexpr int INFILTRATING_QUEEN_BONUS = 75;
constexpr int DEFENSIVE_ATTACKED_PAWN = 37;
constexpr int PAWN_CHAIN_BONUS = 15;
constexpr int UNDEVELOPED_BISHOP_PENALTY = 10;

constexpr int HAS_FREE_RANK_OR_FILE_BONUS = 50;
constexpr int STRONG_BISHOP_BONUS = 35;
constexpr int STRONG_CENTRAL_QUEEN_BONUS = 25;
constexpr int ATTACKING_KNIGHT_BONUS = 75;

constexpr int ADVANCED_PAWN_BONUS = 75;

constexpr int CENTRAL_BISHOP_PENALTY = -75;
constexpr int CENTRAL_KNIGHT_PENALTY = -50;
constexpr int OPPOSITE_BISHOP_PAIRS = 75;
constexpr int PAWN_SHIELD_PAWN_BONUS = 50;
constexpr int KNIGHT_ONE_MOVE_AWAY = 90;
constexpr int KNIGHT_TWO_MOVES_AWAY_BONUS = 115;

constexpr int ROOK_ATTACKS_KING_RAY_BONUS = 45;
constexpr int BLOCKED_KNIGHT_PENALTY = 45;


constexpr int TEAMS[5] = {
    0, 1, -1, 1, -1
};
constexpr int IS_KNIGHT_SWEET_SPOT[2][SQUARE_CNT] = { {
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
}, {
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
} };
constexpr int IS_PREFERRED_KEY_QUEEN_SQUARE[SQUARE_CNT] = {
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1,  0,  0,  0,  0,  1,  0,  1,  1,  0,  1,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  1,  0,  1,  0,  0,  1,  0,  1,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  1,  0,  1,  1,  1,  1,  0,  1,  0,  0,  0, -1,
    -1,  0,  0,  0,  1,  0,  1,  1,  1,  1,  0,  1,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  1,  0,  1,  0,  0,  1,  0,  1,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  1,  0,  1,  1,  0,  1,  0,  0,  0,  0, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
};
constexpr int IS_PREFERRED_KEY_BISHOP_SQUARE[SQUARE_CNT] = {
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,
    -1,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1,
    -1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,
    -1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,
    -1,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1,
    -1,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
};
constexpr int IS_INFILTRATING_PIECE[2][SQUARE_CNT] = { {
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
}, {
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
} };
constexpr int IS_BACK_RANK_PIECE[SQUARE_CNT] = {
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  0,  0,  0,  0,  0,  0,  0,  0, -1, -1, -1, -1,
    -1, -1, -1, -1,  1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
};
constexpr int O_TEAMS[5] = {
    0, 0, 1, 0, 1
};
int inline getRelativeScore(int score, int color) {
	return score * TEAMS[color];
};