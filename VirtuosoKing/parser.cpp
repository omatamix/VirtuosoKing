#include <string>
#include <map>
#include "parser.h"
void Parser::parseFen(std::string fen) {
    fen += "/0-0";
    std::string delimiter = "-";
    size_t pos = 0;
    int counter = 0;
    bool isDone = false;
    std::string token;
    while ((pos = fen.find(delimiter)) != std::string::npos) {
        token = fen.substr(0, pos);
        if (counter == 0) {
            char col = token.at(0);
            col2Mve = getColor(col);
        } else if (counter == 2) {
            for (int p = 0; p < 4; p++) {
                char ck = token.at(p * 2);
                cstSqrs[0][p+1] = ck == '1';
            }
        } else if (counter == 3) {
            for (int i = 0; i < 4; i++) {
                char ck = token.at(i * 2);
                cstSqrs[1][i+1] = ck == '1';
            }
        } else if (counter == 6) {
            if (token.find("enPassant") != std::string::npos) {
                size_t pose = 0;
                int ctr = 0, ptr = -1;
                std::string epky;
                std::string delimitere = "'";
                int length;
                while ((pose = token.find(delimitere)) != std::string::npos) {
                    epky = token.substr(0, pose);
                    if (ctr == 3 || ctr == 5 || ctr == 7 || ctr == 9) {
                        ptr++;
                        if (epky == "") goto k;
                        if (epky.at(2) == ':') length = 1;
                        else length = 2;
                        int file = sqConv[epky.at(0)];
                        int rank = abs(std::stoi(epky.substr(1, length)) - 14);
                        int sq = rank * 16 + file;
                        epSqrs[ptr+1] = sq;
                    }
                    k:
                    ctr++;
                    token.erase(0, pose + delimitere.length());
                }
            } else goto l;
            isDone = true;
        } else if (isDone) {
            l:
            size_t poseBoard = 0, poseKey = 0;
            std::string rankData, keyData;
            std::string delimiterg = "/", delimiterx = ",";
            int initialStartPointer = 1;
            while ((poseBoard = token.find(delimiterg)) != std::string::npos) {
                rankData = token.substr(0, poseBoard);
                rankData += ",x";
                while ((poseKey = rankData.find(delimiterx)) != std::string::npos) {
                    keyData = rankData.substr(0, poseKey);
                    if (keyData == "x") initialStartPointer++;
                    else if (keyData == "X") {
                        colorMailbox[initialStartPointer] = -1;
                        pieceMailbox[initialStartPointer] = -1;
                        initialStartPointer++;
                    } else if (isPiece(keyData)) {
                        int colorToSet = getColor(keyData.at(0));
                        colorMailbox[initialStartPointer] = colorToSet;
                        pieceMailbox[initialStartPointer] = sdConv[keyData];
                        if (sdConv[keyData] == 6) kingsTracker[colorToSet] = initialStartPointer;
                        initialStartPointer++;
                    } else initialStartPointer += std::stoi(keyData);
                    rankData.erase(0, poseKey + delimiterx.length());
                }
                initialStartPointer += 2;
                token.erase(0, poseBoard + delimiterg.length());
            }
            break;
        }
        counter++;
        fen.erase(0, pos + delimiter.length());
    }
}
void Parser::initParser() {
    sqConv['a'] = 1;  sqConv['b'] = 2;  sqConv['c'] = 3;
    sqConv['d'] = 4;  sqConv['e'] = 5;  sqConv['f'] = 6;
    sqConv['g'] = 7;  sqConv['h'] = 8;  sqConv['i'] = 9;
    sqConv['j'] = 10; sqConv['k'] = 11; sqConv['l'] = 12;
    sqConv['m'] = 13; sqConv['n'] = 14; sdConv["rP"] = 1;
    sdConv["rN"] = 2; sdConv["rB"] = 3; sdConv["rR"] = 4;
    sdConv["rQ"] = 5; sdConv["rK"] = 6; sdConv["bP"] = 1;
    sdConv["bN"] = 2; sdConv["bB"] = 3; sdConv["bR"] = 4;
    sdConv["bQ"] = 5; sdConv["bK"] = 6; sdConv["yP"] = 1;
    sdConv["yN"] = 2; sdConv["yB"] = 3; sdConv["yR"] = 4;
    sdConv["yQ"] = 5; sdConv["yK"] = 6; sdConv["gP"] = 1;
    sdConv["gN"] = 2; sdConv["gB"] = 3;
    sdConv["gR"] = 4; sdConv["gQ"] = 5;
    sdConv["gK"] = 6;
}
bool Parser::isPiece(std::string data) {
    char p = data.at(0);
    return p == 'r' || p == 'b' || p == 'y' || p == 'g';
}
int Parser::getColor(char color) {
    if (color == 'R' || color == 'r') return 1;
    else if (color == 'B' || color == 'b') return 2;
    else if (color == 'Y' || color == 'y') return 3;
    else return 4;
}