#include <atomic>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include "defs.h"
#include "eval.h"
#include "parser.h"
#include "pos.h"
#include "preft.h"
#include "search.h"
extern std::atomic<bool> isStop;
extern std::atomic<bool> stopSignal;
std::vector<std::string> split(const std::string& s, char d);
void setFenToSearch(std::string fen, Parser& parser, Position& basePos);
int main() {
    initThreadMemory();
    initTranspositionKeyHandler();
    setNumThreads(DEFAULT_NUM_THREADS);
    std::string input;
    std::vector<std::string> inputVector;
    std::string name = "VirtuosoKing";
    std::string version = "vk-v1.0-beta-prc";
    std::string author = "Nicholas English";
    std::cout << name << " " << version << " by " << author << std::endl;
    std::thread searchThread;
    Parser parser{};
    parser.initParser();
    Position basePos;
    basePos.initBaseGlobalVar();
    while (getline(std::cin, input)) {
        inputVector = split(input, ' ');
        std::cin.clear();
        if (!isStop && input != "stop" && input != "quit") {
            continue;
        }
        if (input == "uci") {
            std::cout << "id name " << name << " " << version << std::endl;
            std::cout << "id author " << author << std::endl;
        } else if (input == "isready") {
            std::cout << "readyok" << std::endl;
        } else if (input == "showmoves") {
            bool checked = basePos.inCheck(basePos.curTurn);
            std::vector<Move> allMoves = basePos.getAllMoves(basePos.curTurn);
            for (const Move& move : allMoves) {
                History oldMoveData = basePos.doMove(move);
                if (checked && (move.moveFlag & CASTLE_OO || move.moveFlag & CASTLE_OOO)) {
                    basePos.undoMove(oldMoveData, move);
                    continue;
                }
                if (basePos.inCheck(oldMoveData.turn) && move.capturedPiece != KING) {
                    basePos.undoMove(oldMoveData, move);
                    continue;
                }
                std::cout << getSanMove(move) << "\n";
                basePos.undoMove(oldMoveData, move);
            }
        } else if (input == "ucinewgame") {
            setFenToSearch(BASE_FEN, parser, basePos);
        } else if (input.substr(0, 8) == "position") {
            std::string fenToRun = "";
            if (input.find("fen") != std::string::npos) {
                fenToRun = inputVector[2];
            }
            setFenToSearch(fenToRun, parser, basePos);
        } else if (input.substr(0, 2) == "go" && isStop) {
            std::vector<std::string>::iterator it;
            if (input.find("ponder") != std::string::npos) {
                startPonder();
            }
            TimeParams timeParams;
            if (input.find("movetime") != std::string::npos && inputVector.size() > 2) {
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
            } else {}
            isStop = false;
            stopSignal = false;
            if (searchThread.joinable()) searchThread.join();
            searchThread = std::thread(search, basePos, timeParams);
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
                if (inputVector.at(2) == "threads") {
                    int threads = std::stoi(inputVector.at(4));
                    if (threads < DEFAULT_NUM_THREADS) {
                        threads = DEFAULT_NUM_THREADS;
                    }
                    if (threads > MAX_NUM_THREADS) {
                        threads = MAX_NUM_THREADS;
                    }
                    setNumThreads(threads);
                } else std::cout << "info string Invalid option." << std::endl;
            }
        }
    }
    return 0;
};
void setFenToSearch(std::string fen, Parser& parser, Position& basePos) {
    for (int i = 0; i < 224; ++i) parser.pieceMailbox[i] = BASE_MAILBOX[i];
    for (int i = 0; i < 224; ++i) parser.colorMailbox[i] = BASE_MAILBOX[i];
    for (int i = 0; i < 5; ++i) parser.epSqrs[i] = 0;
    for (int i = 0; i < 2; ++i)
        for (int k = 0; k < 5; ++k) parser.cstSqrs[i][k] = false;
    for (int i = 0; i < 5; ++i) parser.kingsTracker[i] = 0;
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