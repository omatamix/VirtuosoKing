#pragma once
#include "defs.h"
constexpr int MATERIAL_SCORES[7] = {
	0, 85, 300, 350, 500, 1250, MATE_SCORE
};
constexpr int ADVANCED_PAWN_BONUS = 75;
constexpr int INFILTRATING_QUEEN_BONUS = 50;
constexpr int CENTRAL_BISHOP_PENALTY = -75;
constexpr int CENTRAL_KNIGHT_PENALTY = -50;
constexpr int OPPOSITE_BISHOP_PAIRS = 75;
constexpr int PAWN_SHIELD_PAWN_BONUS = 50;
constexpr int KNIGHT_ONE_MOVE_AWAY = 90;
constexpr int KNIGHT_TWO_MOVE_AWAY = 55;
constexpr int SQUARE_CNT = 224;
constexpr int TEAMS[5] = {
	0, 1, -1, 1, -1
};
int inline getRelativeScore(int score, int color) {
	return score * TEAMS[color];
};