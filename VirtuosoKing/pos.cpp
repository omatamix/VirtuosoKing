#include <iostream>
#include <map>
#include "eval.h"
#include "pos.h"
void Position::setPosData(int nxtCol, int col[224], int pie[224], int ep[5],
    bool cstSc[2][5], int kt[5], int ps, bool setMaterialScore) {
    curTurn = nxtCol;
    for (int i = 0; i < 224; ++i)
        colorMailbox[i] = col[i];
    for (int i = 0; i < 224; ++i)
        pieceMailbox[i] = pie[i];
    for (int i = 0; i < 5; ++i)
        enpassants[i] = ep[i];
    for (int i = 0; i < 2; ++i)
        for (int k = 0; k < 5; ++k) castleTracker[i][k] = cstSc[i][k];
    for (int i = 0; i < 5; ++i)
        kingTracker[i] = kt[i];
    posScore = ps;
    if (setMaterialScore)
        posScore = getMaterialScore();
}
History Position::doMove(Move move) {
    History gameData = {
        posScore,
        curTurn,
        kingTracker[curTurn],
        colorMailbox[move.to],
        enpassants[curTurn],
        enpassants[nextColor(curTurn)],
        colorMailbox[move.from+OFFSETS[curTurn][0]],
        castleTracker[0][curTurn],
        castleTracker[1][curTurn]
    };
    posScore += scoreMove(move);
    posScore = -posScore;
    bool disableCastle = false;
    if (move.moveFlag & CASTLE_OO) {
        pieceMailbox[CASTLING_CK_SQRSK[curTurn][0]+3] = ROOK;
        colorMailbox[CASTLING_CK_SQRSK[curTurn][0]+3] = curTurn;
        pieceMailbox[CASTLING_CK_SQRSK[curTurn][1]+3] = KING;
        colorMailbox[CASTLING_CK_SQRSK[curTurn][1]+3] = curTurn;
        kingTracker[curTurn] = CASTLING_CK_SQRSK[curTurn][1]+3;
        disableCastle = true;
    }
    if (move.moveFlag & CASTLE_OOO) {
        pieceMailbox[CASTLING_CK_SQRSQ[curTurn][0]+3] = ROOK;
        colorMailbox[CASTLING_CK_SQRSQ[curTurn][0]+3] = curTurn;
        pieceMailbox[CASTLING_CK_SQRSQ[curTurn][1]+3] = KING;
        colorMailbox[CASTLING_CK_SQRSQ[curTurn][1]+3] = curTurn;
        kingTracker[curTurn] = CASTLING_CK_SQRSQ[curTurn][1]+3;
        disableCastle = true;
    }
    if (disableCastle) {
        castleTracker[0][curTurn] = false;
        castleTracker[1][curTurn] = false;
        pieceMailbox[move.from] = 0;
        colorMailbox[move.from] = 0;
        pieceMailbox[move.to] = 0;
        colorMailbox[move.to] = 0;
    } else {
        if (move.moved == KING) {
            kingTracker[curTurn] = move.to;
            castleTracker[0][curTurn] = false;
            castleTracker[1][curTurn] = false;
        }
        if (move.moved == ROOK) {
            if (move.from == CST_ROOK_LOCATIONS[0][curTurn]+3) {
                castleTracker[0][curTurn] = false;
            }
            if (move.from == CST_ROOK_LOCATIONS[1][curTurn]+3) {
                castleTracker[1][curTurn] = false;
            }
        }
        if (move.moveFlag & BIG_PAWN) enpassants[curTurn] = move.to - OFFSETS[curTurn][0];
        if (move.moveFlag & EP_CAPTURE) {
            pieceMailbox[move.from + OFFSETS[curTurn][0]] = PIECE_ZERO;
            colorMailbox[move.from + OFFSETS[curTurn][0]] = PIECE_ZERO;
        }
        if (move.moveFlag & PROMOTION || move.moveFlag & PROMOTION_CAPTURE) pieceMailbox[move.to] = move.promotion;
        else pieceMailbox[move.to] = pieceMailbox[move.from];
        colorMailbox[move.to] = colorMailbox[move.from];
        pieceMailbox[move.from] = 0;
        colorMailbox[move.from] = 0;
    }
    curTurn = nextColor(curTurn);
    enpassants[curTurn] = 0;
    return gameData;
}
void Position::undoMove(History historyOfPosition, Move move) {
    castleTracker[0][historyOfPosition.turn] = historyOfPosition.lastCastleK;
    castleTracker[1][historyOfPosition.turn] = historyOfPosition.lastCastleQ;
    enpassants[curTurn] = historyOfPosition.epSquareOT;
    curTurn = historyOfPosition.turn;
    posScore = historyOfPosition.score;
    kingTracker[historyOfPosition.turn] = historyOfPosition.king;
    enpassants[historyOfPosition.turn] = historyOfPosition.epSquare;
    pieceMailbox[move.from] = move.moved;
    colorMailbox[move.from] = historyOfPosition.turn;
    pieceMailbox[move.to] = move.capturedPiece;
    colorMailbox[move.to] = historyOfPosition.capturedColor;
    if (move.moveFlag & CASTLE_OO) {
        pieceMailbox[move.to] = KING;
        colorMailbox[move.to] = historyOfPosition.turn;
        pieceMailbox[CASTLING_CK_SQRSK[historyOfPosition.turn][1]+3] = 0;
        colorMailbox[CASTLING_CK_SQRSK[historyOfPosition.turn][1]+3] = 0;
        pieceMailbox[CASTLING_CK_SQRSK[historyOfPosition.turn][0]+3] = 0;
        colorMailbox[CASTLING_CK_SQRSK[historyOfPosition.turn][0]+3] = 0;
    }
    if (move.moveFlag & CASTLE_OOO) {
        pieceMailbox[move.to] = KING;
        colorMailbox[move.to] = historyOfPosition.turn;
        pieceMailbox[CASTLING_CK_SQRSQ[historyOfPosition.turn][1]+3] = 0;
        colorMailbox[CASTLING_CK_SQRSQ[historyOfPosition.turn][1]+3] = 0;
        pieceMailbox[CASTLING_CK_SQRSQ[historyOfPosition.turn][0]+3] = 0;
        colorMailbox[CASTLING_CK_SQRSQ[historyOfPosition.turn][0]+3] = 0;
    }
    if (move.moveFlag & EP_CAPTURE) {
        pieceMailbox[move.from+OFFSETS[historyOfPosition.turn][0]] = PAWN;
        colorMailbox[move.from+OFFSETS[historyOfPosition.turn][0]] = historyOfPosition.epColor;
    }
}
HistoryNullMove Position::doNullMove() {
    HistoryNullMove nullHistory = {
        posScore,
        curTurn,
        enpassants[1],
        enpassants[2],
        enpassants[3],
        enpassants[4],
    };
    enpassants[1] = 0;
    enpassants[2] = 0;
    enpassants[3] = 0;
    enpassants[4] = 0;
    curTurn = nextColor(curTurn);
    posScore = -posScore;
    return nullHistory;
}
void Position::undoNullMove(HistoryNullMove nullHistory) {
    curTurn = nullHistory.turn;
    posScore = nullHistory.score;
    enpassants[1] = nullHistory.epSquareR;
    enpassants[2] = nullHistory.epSquareB;
    enpassants[3] = nullHistory.epSquareY;
    enpassants[4] = nullHistory.epSquareG;
}
bool Position::inCheck(int color) {
    return isSquareAttacked(kingTracker[color], color);
}
bool Position::isSquareAttacked(int square, int color) {
    for (int start = 0; start < OFFSETS_NUM[10]; ++start) {
        int increment = OFFSETS[10][start];
        for (int n = square + increment; ; n += increment) {
            if (isInvalidSquare(n)) {
                break;
            }
            int pie = pieceMailbox[n];
            if (pie != PIECE_ZERO) {
                if (isOpponentsPiece(n, color)) {
                    if ((abs(increment) == 16 || abs(increment) == 1) &&
                        (pie == ROOK || pie == QUEEN || (pie == KING && n == square + increment)))
                        return true;
                    if (pie == PAWN && n == square + increment) {
                        if (colorMailbox[n] == RED && (increment == 15 || increment == 17)) return true;
                        else if (colorMailbox[n] == BLUE && (increment == 15 || increment == -17)) return true;
                        else if (colorMailbox[n] == YELLOW && (increment == -15 || increment == -17)) return true;
                        else if (colorMailbox[n] == GREEN && (increment == -15 || increment == 17)) return true;
                    }
                    if ((abs(increment) == 15 || abs(increment) == 17) &&
                        (pie == BISHOP || pie == QUEEN || (pie == KING && n == square + increment)))
                        return true;
                    if ((abs(increment) == 18 || abs(increment) == 14 ||
                         abs(increment) == 31 || abs(increment) == 33)
                        && pie == KNIGHT) {
                        return true;
                    }
                }
                break;
            }
            if ((abs(increment) == 18 || abs(increment) == 14 ||
                 abs(increment) == 31 || abs(increment) == 33)) break;
        }
    }
    return false;
}
std::vector<Move> Position::getAllMoves(int color) {
    std::vector<Move> moves;
    int curIndex = -1;
    for (int s = 4; s < 220; ++s) {
        if (isInvalidSquare(s)) continue;
        int pie = pieceMailbox[s];
        if (colorMailbox[s] == color) {
            if (pie == PAWN) {
                int dest = s + OFFSETS[color][0];
                int CAPTURE_BIT;
                int NORMAL_BIT;
                if (isPromotionSquare(dest, color)) {
                    CAPTURE_BIT = PROMOTION_CAPTURE;
                    NORMAL_BIT = PROMOTION;
                } else {
                    CAPTURE_BIT = CAPTURE;
                    NORMAL_BIT = QUIET;
                }
                if (pieceMailbox[dest] == PIECE_ZERO) {
                    curIndex++;
                    moves.push_back({ s, dest, QUEEN, NORMAL_BIT, pie, PIECE_ZERO, PIECE_ZERO, curIndex });
                    dest = dest + OFFSETS[color][0];
                    if (isPawnStartSquare(s, TEAMS[color]) && pieceMailbox[dest] == PIECE_ZERO) {
                        curIndex++;
                        moves.push_back({ s, dest, PIECE_ZERO, BIG_PAWN, pie, PIECE_ZERO, PIECE_ZERO, curIndex });
                    }
                }
                dest = s + OFFSETS[color][1];
                if ((enpassants[OPPOSITES[color][0]] == dest || enpassants[OPPOSITES[color][1]] == dest)
                    && enpassants[color] != dest && pieceMailbox[dest] == PIECE_ZERO) {
                    curIndex++;
                    moves.push_back({ s, dest, PIECE_ZERO, EP_CAPTURE, pie, PAWN, pieceMailbox[s+OFFSETS[color][0]], curIndex });
                }
                if (isOpponentsPiece(dest, color) && !isInvalidSquare(dest)) {
                    curIndex++;
                    moves.push_back({ s, dest, QUEEN, CAPTURE_BIT, pie, pieceMailbox[dest], colorMailbox[dest], curIndex });
                }
                dest = s + OFFSETS[color][2];
                if ((enpassants[OPPOSITES[color][0]] == dest || enpassants[OPPOSITES[color][1]] == dest)
                    && enpassants[color] != dest && pieceMailbox[dest] == PIECE_ZERO) {
                    curIndex++;
                    moves.push_back({ s, dest, PIECE_ZERO, EP_CAPTURE, pie, PAWN, pieceMailbox[s+OFFSETS[color][0]], curIndex });
                }
                if (isOpponentsPiece(dest, color) && !isInvalidSquare(dest)) {
                    curIndex++;
                    moves.push_back({ s, dest, QUEEN, CAPTURE_BIT, pie, pieceMailbox[dest], colorMailbox[dest], curIndex });
                }
            } else {
                for (int start = 0; start < OFFSETS_NUM[pie+3]; ++start) {
                    int increment = OFFSETS[pie+3][start];
                    for (int n = s + increment; ; n += increment) {
                        if (isInvalidSquare(n)) {
                            break;
                        }
                        if (pieceMailbox[n] != PIECE_ZERO) {
                            if (isOpponentsPiece(n, color)) {
                                curIndex++;
                                moves.push_back({ s, n, PIECE_ZERO, CAPTURE, pie, pieceMailbox[n], colorMailbox[n], curIndex });
                            } else if (pieceMailbox[n] == KING) {
                                if (castleTracker[0][color] && CST_ROOK_LOCATIONS[0][color]+3 == s) {
                                    bool passed = true;
                                    for (const int& keySNBA : CASTLING_CK_SQRSK[color]) {
                                        if (isSquareAttacked(keySNBA+3, color)) {
                                            passed = false;
                                            break;
                                        }
                                    }
                                    if (passed) {
                                        curIndex++;
                                        moves.push_back({ s, n, PIECE_ZERO, CASTLE_OO, pie, PIECE_ZERO, PIECE_ZERO, curIndex });
                                    }
                                }
                                if (castleTracker[1][color] && CST_ROOK_LOCATIONS[1][color]+3 == s) {
                                    bool passed = true;
                                    for (const int& keySNBA : CASTLING_CK_SQRSQ[color]) {
                                        if (isSquareAttacked(keySNBA+3, color)) {
                                            passed = false;
                                            break;
                                        }
                                    }
                                    if (passed) {
                                        curIndex++;
                                        moves.push_back({ s, n, PIECE_ZERO, CASTLE_OOO, pie, PIECE_ZERO, PIECE_ZERO, curIndex });
                                    }
                                }
                            }
                            break;
                        } else {
                            curIndex++;
                            moves.push_back({ s, n, PIECE_ZERO, QUIET, pie, pieceMailbox[n], colorMailbox[n], curIndex });
                        }
                        if (pie == KNIGHT || pie == KING) break;
                    }
                }
            }
        }
    }
    return moves;
}
int Position::scoreMove(Move move) {
    int score = 0;
    if (move.moveFlag & EP_CAPTURE)
        score += MATERIAL_SCORES[PAWN];
    if (move.moveFlag & CAPTURE || move.moveFlag & PROMOTION_CAPTURE)
        score += MATERIAL_SCORES[move.capturedPiece];
    if (move.moveFlag & PROMOTION || move.moveFlag & PROMOTION_CAPTURE)
        score += MATERIAL_SCORES[move.promotion];
        score -= MATERIAL_SCORES[move.moved];
    return score;
}
void Position::initBaseGlobalVar() {
    for (int i = 0; i < 224; ++i) pieceMailbox[i] = BASE_MAILBOX[i];
    for (int i = 0; i < 224; ++i) colorMailbox[i] = BASE_MAILBOX[i];
    for (int i = 0; i < 2; ++i)
        for (int k = 0; k < 5; ++k) castleTracker[i][k] = false;
    for (int i = 0; i < 5; ++i) kingTracker[i] = 0;
    for (int i = 0; i < 5; ++i) enpassants[i] = 0;
}
void Position::placePiece(int square, int piece, int color) {
    pieceMailbox[square] = piece;
    colorMailbox[square] = color;
}
void Position::removePiece(int square) {
    pieceMailbox[square] = PIECE_ZERO;
    colorMailbox[square] = PIECE_ZERO;
}
bool Position::isOpponentsPiece(int square, int color) {
    return !(colorMailbox[square] == color || colorMailbox[square] == PARTNERS[color] || colorMailbox[square] == 0);
}
int Position::getStaticEval() {
    int score = 0;
    int ksScore = 0;
    int cnScore = 0;
    int pieceCounter[5] = { 0, 0, 0, 0, 0 };
    int bishopCounter[5] = { 0, 0, 0, 0, 0 };
    for (int square = 4; square < 220; ++square) {
        if (isInvalidSquare(square)) continue;
        if (pieceMailbox[square] == PIECE_ZERO) continue;
        score += getRelativeScore(MATERIAL_SCORES[pieceMailbox[square]], colorMailbox[square]);
        for (int start = 0; start < OFFSETS_NUM[pieceMailbox[square] + 3]; ++start) {
            int increment = OFFSETS[pieceMailbox[square] + 3][start];
            for (int n = square + increment; ; n += increment) {
                if (isInvalidSquare(n)) break;
                if (pieceMailbox[n] != PIECE_ZERO) {
                    if (!isOpponentsPiece(n, colorMailbox[square])) {
                        if (pieceMailbox[n] == PAWN && pieceMailbox[square] == KING) {
                            score += getRelativeScore(50, colorMailbox[square]);
                        }
                    }
                    break;
                }
                score += getRelativeScore(MOBILITY_SCORES[pieceMailbox[square]], colorMailbox[square]);
                if (pieceMailbox[square] == KNIGHT || pieceMailbox[square] == KING) break;
            }
        }
        if (pieceMailbox[square] != PAWN && pieceMailbox[square] != KING) pieceCounter[colorMailbox[square]]++;
        if (pieceMailbox[square] == BISHOP) bishopCounter[colorMailbox[square]]++;
    }
    score += getRelativeScore(IMBALANCE_TABLE[abs(pieceCounter[1] - pieceCounter[3])], 1);
    score += getRelativeScore(IMBALANCE_TABLE[abs(pieceCounter[2] - pieceCounter[4])], 2);
    for (int color = 1; color <= 4; ++color) {
        if (bishopCounter[color] >= 2) score += getRelativeScore(75, color);
    }
    return getRelativeScore(score + ksScore + cnScore, curTurn);
}
int Position::getMaterialScore() {
    int score = 0;
    for (int square = 4; square < 220; ++square) {
        if (isInvalidSquare(square)) continue;
        if (pieceMailbox[square] == PIECE_ZERO) continue;
        score += getRelativeScore(MATERIAL_SCORES[pieceMailbox[square]], colorMailbox[square]);
    }
    return getRelativeScore(score, curTurn);
}
std::string getSanMove(Move move) {
    if (move.moveFlag & CASTLE_OOO) return "O-O-O";
    if (move.moveFlag & CASTLE_OO) return "O-O";
    char seperator = '-';
    if (move.moveFlag & CAPTURE || move.moveFlag & PROMOTION_CAPTURE
                                || move.moveFlag & EP_CAPTURE)
        seperator = 'x';
    std::string sanMove = "";
    if (move.moved != PAWN)
        sanMove.push_back(PIECE_LETTERS[move.moved]);
    sanMove.push_back(LETTER_COORDINATES[move.from]);
    sanMove += NUMBER_COORDINATES[move.from];
    sanMove.push_back(seperator);
    if (move.capturedPiece != PAWN && seperator == 'x')
        sanMove.push_back(PIECE_LETTERS[move.capturedPiece]);
    sanMove.push_back(LETTER_COORDINATES[move.to]);
    sanMove += NUMBER_COORDINATES[move.to];
    return sanMove;
}
std::string getUciMove(Move move) {
    std::string uciMove = "";
    uciMove.push_back(LETTER_COORDINATES[move.from]);
    uciMove += NUMBER_COORDINATES[move.from];
    uciMove.push_back(LETTER_COORDINATES[move.to]);
    uciMove += NUMBER_COORDINATES[move.to];
    return uciMove;
}
void printBoard(Position pos) {
    int pointer = 0;
    std::cout << "+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n";
    for (int i = 0; i < 3; ++i) {
        pointer += 4;
        std::cout << "|----|----|----";
        for (int r = 0; r < 8; ++r) {
            std::cout << "| " << COLORS[pos.colorMailbox[pointer]] << PIECE_LETTERS[pos.pieceMailbox[pointer]] << " ";
            pointer++;
        }
        std::cout << "|----|----|----|\n";
        std::cout << "+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n";
        pointer += 4;
    }
    for (int i = 0; i < 8; ++i) {
        pointer += 1;
        for (int r = 0; r < 14; ++r) {
            std::cout << "| " << COLORS[pos.colorMailbox[pointer]] << PIECE_LETTERS[pos.pieceMailbox[pointer]] << " ";
            pointer++;
        }
        pointer += 1;
        std::cout << "|\n";
        std::cout << "+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n";
    }
    for (int i = 0; i < 3; ++i) {
        pointer += 4;
        std::cout << "|----|----|----";
        for (int r = 0; r < 8; ++r) {
            std::cout << "| " << COLORS[pos.colorMailbox[pointer]] << PIECE_LETTERS[pos.pieceMailbox[pointer]] << " ";
            pointer++;
        }
        pointer += 4;
        std::cout << "|----|----|----|\n";
        std::cout << "+----+----+----+----+----+----+----+----+----+----+----+----+----+----+\n";
    }
}