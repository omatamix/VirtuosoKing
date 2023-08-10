#include "defs.h"
#include "pos.h"
#include "eval.h"
const std::string BASE_FEN = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,8,x,x,x/bR,bP,10,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,10,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,rP,rP,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
constexpr int futilityMoveCount(bool improving, Depth depth) {
	return improving ? (3 + depth * depth) : (3 + depth * depth) / 2;
}
struct SearchParameters {
	int ply;
	int selectiveDepth;
	SearchParameters() {
		reset();
	}
	void reset() {
		ply = 0;
	}
};
struct TimeParams {
	int searchMode;
	int maxAllotment;
	int allotment;
};
enum nodeType : int {
	PV_NODE, ALL_NODE, ROOT_NODE
};
struct SearchPV {
	int pvLength;
	Move pv[MAX_SEARCH_DEPTH + 1];
	SearchPV() {
		pvLength = 0;
	}
};
struct SearchStack {
	int ply;
	int staticEval;
	int moveCount;
	bool ttPv;
};
struct SearchStatistics {
	uint64_t nodes;
	SearchStatistics() {
		reset();
	}
	void reset() {
		nodes = 0;
	}
};
struct ThreadMemory {
	SearchParameters searchParams;
	SearchStack ssInfo[129];
	SearchStatistics searchStats;
	ThreadMemory() {
		for (int i = 0; i < 129; i++) {
			ssInfo[i].ply = i;
		}
	}
	~ThreadMemory() = default;
};
inline int calculateDistanceUntilMate(int curScore) {
	if (curScore > MAX_PLY_MATE_SCORE) return MATE_SCORE - curScore;
	return -MATE_SCORE + curScore;
}
inline void changePV(Move best, SearchPV* parent, SearchPV* child) {
	parent->pv[0] = best;
	for (int i = 0; i < child->pvLength; i++) {
		parent->pv[i + 1] = child->pv[i];
	}
	parent->pvLength = child->pvLength + 1;
}
inline int indexOf(int color, int piece = -1) {
	if (piece == PAWN && color == RED)      return 0;
	if (piece == PAWN && color == BLUE)     return 1;
	if (piece == PAWN && color == YELLOW)   return 2;
	if (piece == PAWN && color == GREEN)    return 3;
	if (color == RED && piece == KNIGHT)    return 4;
	if (color == RED && piece == BISHOP)    return 5;
	if (color == RED && piece == ROOK)      return 6;
	if (color == RED && piece == QUEEN)     return 7;
	if (color == RED && piece == KING)      return 8;
	if (color == BLUE && piece == KNIGHT)   return 9;
	if (color == BLUE && piece == BISHOP)   return 10;
	if (color == BLUE && piece == ROOK)     return 11;
	if (color == BLUE && piece == QUEEN)    return 12;
	if (color == BLUE && piece == KING)     return 13;
	if (color == YELLOW && piece == KNIGHT) return 14;
	if (color == YELLOW && piece == BISHOP) return 15;
	if (color == YELLOW && piece == ROOK)   return 16;
	if (color == YELLOW && piece == QUEEN)  return 17;
	if (color == YELLOW && piece == KING)   return 18;
	if (color == GREEN && piece == KNIGHT)  return 19;
	if (color == GREEN && piece == BISHOP)  return 20;
	if (color == GREEN && piece == ROOK)    return 21;
	if (color == GREEN && piece == QUEEN)   return 22;
	if (color == GREEN && piece == KING)    return 23;
	else                                    return 24;
};
uint64_t getNodes();
int getSelectiveDepth();
void initTranspositionKeyHandler();
void initThreadMemory();
void search(Position pos, TimeParams timeParams);
void searchRoot(Position pos, std::vector<Move> legalMoves, TimeParams timeParams, int threadID);
void searchRootPVS(Position pos, std::vector<Move> legalMoves, Depth depth, int alpha,
	int beta, int* bestMoveIndex, int* bestScore, int threadID, SearchPV* pvLine);
template<nodeType curNode>
int pvs(Position pos, Depth depth, int alpha, int beta, int threadID, bool isCutNode,
	bool gaveCheck, SearchStack* ssi, SearchPV* pvLine, int extended, Move lastMovePlayed);
int quiescence(Position pos, int plies, int alpha, int beta, int threadID);
std::string getPvLine(SearchPV* pvLine);
void startPonder();
void stopPonder();
void setNumThreads(int n);
int scoreMate(bool isInCheck, int plies);
uint64_t computeHash(Position pos);