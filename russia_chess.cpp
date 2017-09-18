#include "russia_chess.hpp"

#include <sys/resource.h>

void encodeTest() {
    Container pieces = {0, 5, 0, 0, 0, 0};

    int encodeNum = encodePieces(pieces);
    std::cout << encodeNum << std::endl;

    auto result = decodeNum(encodeNum);
    for(const auto& v : result)
        std::cout << v << " ";
    std::cout << std::endl;
}

void setStackSize(int megabytes) {
    struct rlimit rlim;

    getrlimit( RLIMIT_STACK, &rlim );
    printf( "Current Stack Size : [%d] Max Current Stack Size : [%d]\n", rlim.rlim_cur, rlim.rlim_max );

    rlim.rlim_cur = (1024 * 1024 * megabytes);
    rlim.rlim_max = (1024 * 1024 * megabytes);
    setrlimit( RLIMIT_STACK, &rlim );
    printf( "Current Stack Size : [%d] Max Current Stack Size : [%d]\n", rlim.rlim_cur, rlim.rlim_max );
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

State dfs(int ourNum, int otherNum) {
    bool winningCond = false;
    TurnState ret{ 0, State::Processing };

    auto it = stateMap[ourNum].find(otherNum);
    if (it != stateMap[ourNum].end())
        return it->second;

    std::vector<int> nextNums;
    for (int target : targetPriority) {
        for (int point : pointPriority) {
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

            stateMap[sameOurNum][sameOtherNum] = State::Processing;
        }

        for (int nextNum : nextNums) {
            auto temp = dfs(otherNum, nextNum);

            if (temp == State::Lose) {
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

            stateMap[sameOurNum][sameOtherNum] = State::Win;
            nextMap[sameOurNum][sameOtherNum] = sameNextNum;
        }
    } else {
        for (int i=0; i<sameMapSet.size(); i++) {
            int sameOurNum = sameOurNums[i];
            int sameOtherNum = sameOtherNums[i];

            stateMap[sameOurNum][sameOtherNum] = State::Lose;
        }
    }

    return ret.state;
}

void calcAll() {
    if (dfs(0, 5) == State::Win) {
        std::cout << "Second win" << std::endl;
    } else {
        std::cout << "First win" << std::endl;
    }
}

int main() {
    setStackSize(1024);
    initPointDict();
    encodeTest();
    calcAll();
}
