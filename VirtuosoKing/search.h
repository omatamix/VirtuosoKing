// 
// 
// int dest = s + OFFSETS[color][0];
// int CAPTURE_BIT;
// int NORMAL_BIT;
// if (isPromotionSquare(dest, color)) {
//     CAPTURE_BIT = PROMOTION_CAPTURE;
//     NORMAL_BIT = PROMOTION;
// }
// else {
//     CAPTURE_BIT = CAPTURE;
//     NORMAL_BIT = QUIET;
// }
// if (pieceMailbox[dest] == PIECE_ZERO) {
//     curIndex++;
//     moves.push_back({ s, dest, QUEEN, NORMAL_BIT, pie, PIECE_ZERO, PIECE_ZERO, curIndex });
//     dest = dest + OFFSETS[color][0];
//     if (isPawnStartSquare(s, TEAMS[color]) && pieceMailbox[dest] == PIECE_ZERO) {
//         curIndex++;
//         moves.push_back({ s, dest, PIECE_ZERO, BIG_PAWN, pie, PIECE_ZERO, PIECE_ZERO, curIndex });
//     }
// }
// dest = s + OFFSETS[color][1];
// if ((enpassants[OPPOSITES[color][0]] == dest || enpassants[OPPOSITES[color][1]] == dest)
//     && enpassants[color] != dest && pieceMailbox[dest] == PIECE_ZERO) {
//     curIndex++;
//     moves.push_back({ s, dest, PIECE_ZERO, EP_CAPTURE, pie, PAWN, pieceMailbox[s + OFFSETS[color][0]], curIndex });
// }
// if (isOpponentsPiece(dest, color) && !isInvalidSquare(dest)) {
//     curIndex++;
//     moves.push_back({ s, dest, QUEEN, CAPTURE_BIT, pie, pieceMailbox[dest], colorMailbox[dest], curIndex });
// }
// dest = s + OFFSETS[color][2];
// if ((enpassants[OPPOSITES[color][0]] == dest || enpassants[OPPOSITES[color][1]] == dest)
//     && enpassants[color] != dest && pieceMailbox[dest] == PIECE_ZERO) {
//     curIndex++;
//     moves.push_back({ s, dest, PIECE_ZERO, EP_CAPTURE, pie, PAWN, pieceMailbox[s + OFFSETS[color][0]], curIndex });
// }
// if (isOpponentsPiece(dest, color) && !isInvalidSquare(dest)) {
//     curIndex++;
//     moves.push_back({ s, dest, QUEEN, CAPTURE_BIT, pie, pieceMailbox[dest], colorMailbox[dest], curIndex });
// }