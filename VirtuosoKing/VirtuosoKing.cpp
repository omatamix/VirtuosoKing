#include <atomic>
#include <cstring>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <thread>
#include "defs.h"
#include "eval.h"
#include "parser.h"
#include "pos.h"
#include "preft.h"
#include "search.h"
#include "tt.h"
extern std::atomic<bool> isStop;
extern std::atomic<bool> stopSignal;
std::vector<std::string> split(const std::string& s, char d);
void setFenToSearch(std::string fen, Parser& parser, Position& basePos);
int main(int argc, char** argv) {
    initReductionTable();
    initTranspositionKeyHandler();
    std::string input;
    std::vector<std::string> inputVector;
    std::string name = "VirtuosoKing";
    std::string version = "vk-v-beta-prc";
    std::string author = "Nicholas English";
    std::cout << name << " " << version << " by " << author << std::endl;
    std::thread searchThread;
    Parser parser{};
    parser.initParser();
    Position basePos;
    basePos.initBaseGlobalVar();
    Search search;
    while (getline(std::cin, input)) {
        inputVector = split(input, ' ');
        std::cin.clear();
        if (!isStop && input != "stop" &&
                       input != "quit" &&
                       input != "ponderhit") {
            continue;
        }
        if (input == "uci") {
            std::cout << "id name " << name << " " << version << std::endl;
            std::cout << "id author " << author << std::endl;
            std::cout << "option name hash type spin default " << DEFAULT_TT_SIZE
                      << " min " << MIN_TT_SIZE << " max " << MAX_TT_SIZE << std::endl;
            std::cout << "option name ponder type check default false" << std::endl;
            std::cout << "uciok" << std::endl;
        } else if (input == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (input == "ucinewgame") {
            setFenToSearch(BASE_FEN, parser, basePos);
        } else if (input.substr(0, 8) == "position") {
            std::string fenToRun = "";
            if (input.find("fen") != std::string::npos) {
                fenToRun = inputVector[2];
            }
            if (fenToRun == "") {
                fenToRun = BASE_FEN;
                setFenToSearch(fenToRun, parser, basePos);
            } else {
                setFenToSearch(fenToRun, parser, basePos);
            }
            if (inputVector[2] == "moves" ||
                inputVector[3] == "moves") {
                int vectPointer;
                if (inputVector[2] == "moves") vectPointer = 3;
                else vectPointer = 4;
                while (vectPointer < inputVector.size()) {
                    std::string uciMove = inputVector[vectPointer];
                    int from, to;
                    parser.getSqaures(from, to, uciMove);
                    int moveFlag = QUIET;
                    if (basePos.pieceMailbox[to] != PIECE_ZERO) {
                        if (basePos.pieceMailbox[from] == PAWN &&
                            isPromotionSquare(to, basePos.curTurn)) {
                            moveFlag = PROMOTION_CAPTURE;
                        }
                        moveFlag = CAPTURE;
                    } else if (basePos.pieceMailbox[from] == PAWN) {
                        if (isPromotionSquare(to, basePos.curTurn)) {
                            moveFlag = PROMOTION;
                        } else if (std::find(std::begin(basePos.enpassants),
                            std::end(basePos.enpassants), to) != std::end(basePos.enpassants)) {
                            moveFlag = EP_CAPTURE;
                        } else if (abs(to - from) == 32 || abs(to - from) == 2) {
                            moveFlag = BIG_PAWN;
                        }
                    } else if (basePos.pieceMailbox[to] == KING &&
                        basePos.colorMailbox[to] == basePos.curTurn) {
                        if (std::find(std::begin(CST_ROOK_LOCATIONS[0]),
                            std::end(CST_ROOK_LOCATIONS[0]), to) != std::end(CST_ROOK_LOCATIONS[0])) {
                            moveFlag = CASTLE_OO;
                        } else {
                            moveFlag = CASTLE_OOO;
                        }
                    }
                    Move pushableMove{ from, to, QUEEN, moveFlag, basePos.pieceMailbox[from],
                        ((moveFlag & EP_CAPTURE) ? PAWN : basePos.pieceMailbox[to]),
                        ((moveFlag & EP_CAPTURE) ? basePos.colorMailbox[from +
                            OFFSETS[basePos.curTurn][0]]
                        : basePos.colorMailbox[to]), -1 };
                    basePos.doMove(pushableMove);
                    vectPointer++;
                }
            }
        } else if (input.substr(0, 2) == "go" && isStop) {
            std::vector<std::string>::iterator it;
            TimeParams timeParams;
            if (input.find("ponder") != std::string::npos) {
                startPonder();
                timeParams.searchMode = DEPTH;
                timeParams.allotment = PIECE_ZERO;
            } else if (input.find("movetime") != std::string::npos && inputVector.size() > 2) {
                timeParams.searchMode = MOVETIME;
                it = find(inputVector.begin(), inputVector.end(), "movetime");
                it++;
                timeParams.allotment = std::stoi(*it);
            } else if (input.find("depth") != std::string::npos && inputVector.size() > 2) {
                timeParams.searchMode = DEPTH;
                it = find(inputVector.begin(), inputVector.end(), "depth");
                it++;
                timeParams.allotment = MAX_SEARCH_DEPTH < std::stoi(*it) ? MAX_SEARCH_DEPTH : std::stoi(*it);
            } else if (input.find("infinite") != std::string::npos) {
                timeParams.searchMode = DEPTH;
                timeParams.allotment = MAX_SEARCH_DEPTH;
            } else if (input.find("nodes") != std::string::npos && inputVector.size() > 2) {
                it = find(inputVector.begin(), inputVector.end(), "nodes");
                it++;
                timeParams.searchMode = DEPTH;
                timeParams.allotment = MAX_SEARCH_DEPTH;
                timeParams.limitNodes = true;
                timeParams.maxNodes = std::stoi(*it);
            } else {
                timeParams.searchMode = DEPTH;
                timeParams.allotment = MAX_SEARCH_DEPTH;
            }
            stopSignal = isStop = false;
            if (searchThread.joinable()) searchThread.join();
            searchThread = std::thread(&Search::getBestMove, &search, basePos, timeParams);
        } else if (input == "ponderhit") {
            stopPonder();
        } else if (input == "stop") {
            stopPonder();
            isStop = true;
            stopSignal = true;
            if (searchThread.joinable()) searchThread.join();
        } else if (input == "quit") {
            stopPonder();
            isStop = true;
            stopSignal = true;
            if (searchThread.joinable()) searchThread.join();
            break;
        } else if (input.substr(0, 9) == "setoption" && inputVector.size() >= 5) {
            if (inputVector.at(1) != "name" || inputVector.at(3) != "value") {
                std::cout << "info string Invalid option format." << std::endl;
            } else {
                if (inputVector.at(2) == "hash") {
                    size_t hashSize = static_cast<size_t>(std::stoi(inputVector.at(4)));
                    if (hashSize < MIN_TT_SIZE) hashSize = MIN_TT_SIZE;
                    if (hashSize > MAX_TT_SIZE) hashSize = MAX_TT_SIZE;
                    setHashSize(hashSize);
                    search.initTTTable();
                } else if (inputVector.at(2) == "ponder") {
                    // do nothing
                } else if (inputVector.at(2) == "style") {
                    size_t hashSize = static_cast<size_t>(std::stoi(inputVector.at(4)));
                    if (hashSize < MIN_TT_SIZE) hashSize = MIN_TT_SIZE;
                    if (hashSize > MAX_TT_SIZE) hashSize = MAX_TT_SIZE;
                    setHashSize(hashSize);
                } else std::cout << "info string Invalid option." << std::endl;
            }
        }
    }
    return 0;
};
void setFenToSearch(std::string fen, Parser& parser, Position& basePos) {
    std::copy(std::begin(BASE_MAILBOX), std::end(BASE_MAILBOX), std::begin(parser.pieceMailbox));
    std::copy(std::begin(BASE_MAILBOX), std::end(BASE_MAILBOX), std::begin(parser.colorMailbox));
    std::fill(std::begin(parser.epSqrs), std::end(parser.epSqrs), 0);
    for (auto& cstSqr : parser.cstSqrs) {
        std::fill(std::begin(cstSqr), std::end(cstSqr), false);
    }
    std::fill(std::begin(parser.kingsTracker), std::end(parser.kingsTracker), 0);
    parser.parseFen(fen);
    basePos.setPosData(parser.col2Mve, parser.colorMailbox, parser.pieceMailbox, parser.epSqrs,
        parser.cstSqrs, parser.kingsTracker, 0, true);
}
std::vector<std::string> split(const std::string& s, char d) {
    std::vector<std::string> v;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, d))
        v.push_back(item);
    return v;
}