#pragma once
constexpr int MOVE_PV               =  (1 << 19);
constexpr int MOVE_GIVES_CHECK       = (1 << 18);
constexpr int MOVE_CAPTURE_KILLER   =  (1 << 17);
constexpr int MOVE_PROMOTION        =  (1 << 16);
constexpr int MOVE_CAPTURE          =  (1 << 15);
constexpr int MOVE_MATE_KILLER      =  (1 << 14);
constexpr int MOVE_KILLER           =  (1 << 13);
constexpr int MOVE_KILLER_SECONDARY =  (1 << 12);
constexpr int MOVE_GREAT_MOVE       =  (1 << 11);
constexpr int MOVE_QUIET            = -(1 << 16);
constexpr int INVALID_SCORE = -99999;
constexpr int MATE_THRESHOLD = 89999;
constexpr size_t MAX_TT_SIZE = 10000000;
constexpr int DEFAULT_ASPIRATION_WIDTH = 50;
constexpr int MULTI_CUT_REDUCTION = 3;
constexpr int MULTICUT_MAX_MOVES_SEARCHING = 4;
constexpr int MULTICUT_CUTOFFS_NEEDED = 3;
constexpr int MAX_QSEARCH_PLY = 10;
const std::string BASE_FEN = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,8,x,x,x/bR,bP,10,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,10,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,rP,rP,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_1 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,1,yP,yP,yP,yP,yP,x,x,x/x,x,x,2,yP,5,x,x,x/bR,bP,9,gN,gP,gR/bN,bP,10,gP,1/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,2,bP,8,gP,gB/bN,bP,10,gP,gN/bR,bP,2,rP,7,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,1,rP,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_2 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,1,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,yN,7,x,x,x/bR,bP,10,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,2,bP,8,gP,gK/bK,bP,9,gP,1,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,2,rP,7,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,1,rP,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_3 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,1,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,5,yN,2,x,x,x/bR,bP,bN,8,gN,gP,gR/1,bP,10,gP,1/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,10,gP,gR/x,x,x,2,rP,5,x,x,x/x,x,x,rP,rP,1,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_4 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,1,yP,yP,yP,yP,yP,x,x,x/x,x,x,8,x,x,x/bR,bP,3,yP,6,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,1,bP,9,gP,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,4,rP,4,gP,1,gR/x,x,x,8,x,x,x/x,x,x,rP,rP,rP,1,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_5 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,1,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,8,x,x,x/bR,bP,2,yP,7,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,2,bP,8,gP,gK/bK,bP,9,gP,1,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,10,gP,gR/x,x,x,2,rN,5,x,x,x/x,x,x,rP,rP,rP,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,1,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_6 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,1,yP,yP,x,x,x/x,x,x,5,yP,2,x,x,x/bR,bP,10,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,9,gP,1,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,2,bP,8,gP,gR/x,x,x,3,rP,4,x,x,x/x,x,x,rP,rP,rP,1,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_7 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,1,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,yP,7,x,x,x/bR,bP,10,gP,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,bP,10,gP,gB/bN,bP,9,gP,1,gN/bR,1,bP,1,rP,7,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,1,rP,rP,rP,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_8 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,yN,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,1,x,x,x/x,x,x,7,yP,x,x,x/bR,bP,8,gP,2,gR/bN,bP,10,gP,gN/bB,bP,10,gP,gB/bQ,bP,10,gP,gK/bK,bP,10,gP,gQ/bB,2,bP,8,gP,gB/bN,bP,10,gP,gN/bR,bP,5,rP,4,gP,gR/x,x,x,8,x,x,x/x,x,x,rP,rP,rP,rP,1,rP,rP,rP,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
const std::string ST_FEN_9 = "R-0,0,0,0-1,1,1,1-1,1,1,1-0,0,0,0-0-x,x,x,yR,1,yB,yK,yQ,yB,yN,yR,x,x,x/x,x,x,yP,yP,yP,yP,yP,yP,yP,yP,x,x,x/x,x,x,2,yN,5,x,x,x/bR,bP,10,gP,gR/bN,bP,10,gP,gN/bB,2,bP,8,gP,gB/bQ,bP,10,gP,gK/bK,bP,8,gP,2,gQ/bB,bP,10,gP,gB/bN,bP,10,gP,gN/bR,bP,10,gP,gR/x,x,x,7,rP,x,x,x/x,x,x,rP,rP,rP,rP,rP,rP,rP,1,x,x,x/x,x,x,rR,rN,rB,rQ,rK,rB,rN,rR,x,x,x";
struct SearchPV {
	int pvLength;
	Move pv[MAX_MOVES - 1];
	SearchPV() {
		pvLength = 0;
	}
};
constexpr int futilityMoveCount(bool improving, Depth depth) {
	return improving ? (1 + depth * depth) : (2 + depth * depth) / 2;
}
constexpr int RAZOR_MARGIN = 300;
constexpr int FIXED_NULL_MOVE_REDUCTION = 3;
constexpr int LMP_TABLE[2][9] = {
	{ 0, 3, 4, 6,  10, 14, 16, 19, 22, },
	{ 0, 7, 9, 10, 15, 18, 20, 26, 31, }
};
inline void changePV(Move best, SearchPV* parent, SearchPV* child) {
	parent->pv[0] = best;
	for (int i = 0; i < child->pvLength; i++) {
		parent->pv[i + 1] = child->pv[i];
	}
	parent->pvLength = child->pvLength + 1;
}
static uint64_t get64rand() {
	return (((uint64_t)rand() << 0) & 0x000000000000FFFFull) | (((uint64_t)rand() << 16) & 0x00000000FFFF0000ull) |
          (((uint64_t)rand() << 32) & 0x0000FFFF00000000ull) | (((uint64_t)rand() << 48) & 0xFFFF000000000000ull);
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
}
struct TimeParams {
	int searchMode;
	int maxAllotment;
	int allotment;
	int maxNodes;
	bool limitNodes = false;
	bool isShortSearch = false;
};
struct SearchStackInfo {
	int ply;
	int moveCount = 0;
	int staticEval = -321114;
	int cutoffCnt = 0;
	bool ttPv = false;
};
uint64_t getTimeElapsed(ChessTime startTime);
void initReductionTable();
void initTranspositionTable();
void initTranspositionKeyHandler();
std::string getPvLine(SearchPV pvLine);
uint64_t computeHash(Position pos);
void clearHistoryScores(bool decayScores);
void resetKillers();
void clearTranspositionTables();
void calculateDistanceToMate(int score);
struct Search {
	bool isTester = false;
    ChessTime _startTime;
    uint64_t _timeLimit;
	size_t lastIndexAccessed = 0;
    int curPly = 0;
	Move bestMoveForPos{};
	int bestScoreOverall = 0;
    int maxDepthReached = 0;
    uint64_t nodeCounter = 0;
	int _selDepth = 0;
    int aspirationWidth = DEFAULT_ASPIRATION_WIDTH;
	Search() {
		clearHistoryScores(false);
		resetKillers();
	}
	int see(Position pos, int square);
	int seeCapture(Position pos, Move captureMove);
	static int scoreMove(const Move& currentOrNext);
	static bool compareMoves(const Move& current, const Move& next);
	void getBestMove(Position pos, TimeParams timeParams);
    int searchPVS(Position pos, SearchStackInfo* stack, int depth, int alpha,
        int beta, SearchPV* pvLine, int* bestMoveIndex);
    int pvSearch(Position pos, SearchStackInfo* stack, int depth, int alpha, int beta,
        SearchPV* pvLine, bool is_pv, bool is_cut, bool nullMoveAllowed);
	int qsearch(Position pos, SearchStackInfo* stack, int alpha, int beta, int depth,
		bool lastMoveWasCheck = false);
};