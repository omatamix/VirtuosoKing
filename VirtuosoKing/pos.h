#pragma once
#pragma warning(disable : 26812)
#include <cstdint>
#include <string>
#include <vector>
#include "defs.h"
#include <map>
enum Piece : int {
    PAWN = 1, KNIGHT, BISHOP, ROOK, QUEEN, KING
};
enum Color : int {
    RED = 1, BLUE, YELLOW, GREEN
};
enum MoveFlags : int {
    QUIET, BIG_PAWN, CAPTURE, EP_CAPTURE = 4,
    PROMOTION = 8, PROMOTION_CAPTURE = 16,
    CASTLE_OO = 32, CASTLE_OOO = 64,
};
inline bool isInvalidSquare(int square) {
    return square < 1 || BASE_MAILBOX[square] == -1 || square > 220;
}
inline int nextColor(int color) {
    return (color % 4) + 1;
}
inline int getRank(int square) {
    return square >> 4;
};
inline int getFile(int square) {
    return square & 15;
};
inline int qualifiesForPawnExtension(int square, int color) {
    switch (color) {
    case RED:
        return getRank(square) <= 7;
        break;
    case BLUE:
        return getFile(square) >= 7;
        break;
    case YELLOW:
        return getRank(square) >= 6;
        break;
    case GREEN:
        return getFile(square) <= 8;
        break;
    default:
        return false;
    }
}
inline bool isPromotionSquare(int square, int color) {
    switch (color) {
    case RED:
        return getRank(square) == 3;
        break;
    case BLUE:
        return getFile(square) == 11;
        break;
    case YELLOW:
        return getRank(square) == 10;
        break;
    case GREEN:
        return getFile(square) == 4;
        break;
    default:
        return false;
    }
};
inline bool isLightSquared(int square) {
    return (square % 2 == 0);
}
inline bool isPawnStartSquare(int square, int team) {
    return team == 1 ? (getRank(square) == 1 || getRank(square) == 12) :
                       (getFile(square) == 2 || getFile(square) == 13);
};
struct HistoryNullMove {
    int score; int turn;
    int epSquareR; int epSquareB;
    int epSquareY; int epSquareG;
};
constexpr uint64_t INVALID_DATA = 0;
struct Move {
    int from;          // ranges from 0 to 224.
    int to;            // ranges from 0 to 224.
    int promotion;     // ranges from 0 to 6.
    int moveFlag;      // ranges from 0 to 64.
    int moved;         // ranges from 0 to 6.
    int capturedPiece; // ranges from 0 to 6;
    int capturedColor; // ranges from 0 to 4;
    int curPieceIndex; // ranges from 0 to 256;
    uint64_t encode() const {
        uint64_t encoded = 0;
        encoded |= static_cast<uint64_t>(from);
        encoded |= static_cast<uint64_t>(to) << 8;
        encoded |= static_cast<uint64_t>(promotion) << 16;
        encoded |= static_cast<uint64_t>(moveFlag) << 19;
        encoded |= static_cast<uint64_t>(moved) << 26;
        encoded |= static_cast<uint64_t>(capturedPiece) << 29;
        encoded |= static_cast<uint64_t>(capturedColor) << 32;
        encoded |= static_cast<uint64_t>(curPieceIndex) << 35;
        return encoded;
    }
    void decode(uint64_t value) {
        from = value & 0xFF;
        to = (value >> 8) & 0xFF;
        promotion = (value >> 16) & 0x7;
        moveFlag = (value >> 19) & 0x7F;
        moved = (value >> 26) & 0x7;
        capturedPiece = (value >> 29) & 0x7;
        capturedColor = (value >> 32) & 0x7;
        curPieceIndex = (value >> 35) & 0x1FF;
    }
};
enum GamePhase {
    OPENING, MIDDLEGAME, ENDGAME
};
struct History {
    int score;                // ranges from 0 to 99999.
    int turn;                 // ranges from 0 to 4.
    int king;                 // ranges from 0 to 224.
    int capturedColor;        // ranges from 0 to 6.
    int epSquare;             // ranges from 0 to 224.
    int epSquareOT;           // ranges from 0 to 224.
    int epColor;              // ranges from 0 to 6.
    bool lastCastleK = false; // ranges from 0 to 1.
    bool lastCastleQ = false; // ranges from 0 to 1.
    // uint64_t encode() const {
    //     uint64_t encoded = 0;
    //     encoded |= static_cast<uint64_t>(score) & 0x1FFFF;
    //     encoded |= static_cast<uint64_t>(turn) << 17;
    //     encoded |= static_cast<uint64_t>(king) << 20;
    //     encoded |= static_cast<uint64_t>(capturedColor) << 28;
    //     encoded |= static_cast<uint64_t>(epSquare) << 31;
    //     encoded |= static_cast<uint64_t>(epSquareOT) << 39;
    //     encoded |= static_cast<uint64_t>(epColor) << 47;
    //     encoded |= static_cast<uint64_t>(lastCastleK) << 50;
    //     encoded |= static_cast<uint64_t>(lastCastleQ) << 51;
    //     return encoded;
    // }
    // void decode(uint64_t value) {
    //     score = value & 0x1FFFF;
    //     turn = (value >> 17) & 0x7;
    //     king = (value >> 20) & 0xFF;
    //     capturedColor = (value >> 28) & 0x7;
    //     epSquare = (value >> 31) & 0xFF;
    //     epSquareOT = (value >> 39) & 0xFF;
    //     epColor = (value >> 47) & 0x7;
    //     lastCastleK = static_cast<bool>((value >> 50) & 0x1);
    //     lastCastleQ = static_cast<bool>((value >> 51) & 0x1);
    // }
};
struct SmallMove {
    int from;
    int to;
    int promotion;
    int flags;
};
struct Position {
public:
    int curTurn = 1;
    int posScore = 0;
    int kingTracker[5];
    bool castleTracker[2][5];
    int colorMailbox[224];
    int pieceMailbox[224];
    int enpassants[5];
    bool isEndgame = false;
    Position() {
        initBaseGlobalVar();
    }
    void setPosData(int nxtCol, int col[224], int pie[224], int ep[5],
        bool cstSc[2][5], int kt[5], int ps, bool setMaterialScore);
    History doMove(Move move);
    void undoMove(History historyOfPosition, Move move);
    HistoryNullMove doNullMove();
    void undoNullMove(HistoryNullMove nullHistory);
    int scoreMove(Move move);
    void initBaseGlobalVar();
    int getMaterialScore();
};
GamePhase getCurrentGamePhase();
bool inCheck(const Position& pos, int color);
bool isSquareAttacked(const Position& pos, int square, int color);
size_t allocateMoves(const Position& pos, int ply);
void getAllocatedMove(uint64_t& encodedMove, int ply, int curIndex);
void clearAllocatedMoves();
int getEval(const Position& pos);
std::pair<int, int> getSmallestAttacker(const Position& pos, int square, int color);
std::string getSanMove(Move move);
std::string getUciMove(Move move);
void printBoard(Position pos);