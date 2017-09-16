#include "russia_chess.hpp"

void encodeTest() {
    Container pieces = {8, 9, 6, 7, 1, 2};

    int encodeNum = encodePieces(pieces);
    std::cout << encodeNum << std::endl;

    auto result = decodeNum(encodeNum);
    for(const auto& v : result)
        std::cout << v << " ";
    std::cout << std::endl;
}

TurnState executeTurn(int ourNum, int otherNum, Move m) {
    TurnState ret;
    Container ours = decodeNum(ourNum);
    Container others = decodeNum(otherNum);

    if (not checkValid(ours, others, m)) {
        ret.state = State::Notvalid;
        return ret;
    }

    ours[m.target] = m.point;
    ret.nextNum = encodePieces(ours);

    if (checkWinning(ours, others, m))
        ret.state = State::Win;

    return ret;
}

TurnState dfs(int ourNum, int otherNum) {
    bool winningCond = false;
    TurnState ret{ 0, State::Processing };

    auto it = turnStates[ourNum].find(otherNum);
    if (it != turnStates[ourNum].end())
        return it->second;

    std::vector<int> nextNums;
    for (int target=0; target < pieceSize; target++) {
        for (int point=1; point < mapSize; point++) {
            Move m = { target, point };
            TurnState temp = executeTurn(ourNum, otherNum, m);

            if (temp.state == State::Notvalid) {
                continue;
            } else if (temp.state == State::Normal) {
                nextNums.push_back(temp.nextNum);
            } else if (temp.state == State::Win) {
                ret = temp;
                winningCond = true;
                break;
            }
        }
        if (winningCond)
            break;
    }

    auto sameOurNums = samePieces(ourNum);
    auto sameOtherNums = samePieces(otherNum);

    if (not winningCond) {
        for (int i=0; i<sameMapSet.size(); i++) {
            int sameOurNum = sameOurNums[i];
            int sameOtherNum = sameOtherNums[i];

            turnStates[sameOurNum][sameOtherNum] =
                { 0, State::Processing };
        }

        for (int nextNum : nextNums) {
            auto temp = dfs(otherNum, nextNum);

            if (temp.state == State::Lose) {
                ret = { nextNum, State::Win };
                winningCond = true;
                break;
            }
        }
    }

    if (winningCond) {
        auto sameNextNums = samePieces(ret.nextNum);
        for (int i=0; i<sameMapSet.size(); i++) {
            int sameOurNum = sameOurNums[i];
            int sameOtherNum = sameOtherNums[i];
            int sameNextNum = sameNextNums[i];

            turnStates[sameOurNum][sameOtherNum] =
                { sameNextNum, State::Win };
        }
    } else {
        for (int i=0; i<sameMapSet.size(); i++) {
            int sameOurNum = sameOurNums[i];
            int sameOtherNum = sameOtherNums[i];

            turnStates[sameOurNum][sameOtherNum] =
                { 0, State::Lose };
        }
    }

    return ret;
}


void calcAll() {
    dfs(0, 0);
}

int main() {
    initPointDict();
    encodeTest();
    calcAll();
}
