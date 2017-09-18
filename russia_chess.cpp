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

    m.origin = ours[m.target];
    ours[m.target] = m.point;
    ret.nextNum = encodePieces(ours);

    short priority = 0;
    if (checkWinning(ours, others, m, priority)) {
        ret.state = State::Win;
    } else {
        ret.priority = priority;
    }

    return ret;
}

State dfs(int ourNum, int otherNum) {
    State ret = State::Processing;
    int nextNum = 0;
    bool winningCond = false;

    auto it = stateMap[ourNum].find(otherNum);
    if (it != stateMap[ourNum].end())
        return it->second;

    static int count = 0;
    std::cout << ++count << std::endl;

    std::vector<NextInfo> nexts;
    for (int target : targetPriority) {
        for (int point : pointPriority) {
            Move m = { target, point, 0 };
            TurnState temp = executeTurn(ourNum, otherNum, m);

            if (temp.state == State::Notvalid) {
                continue;
            } else if (temp.state == State::Normal) {
                if (temp.priority < 0)
                    continue;
                nexts.push_back({temp.priority, temp.nextNum});
            } else if (temp.state == State::Win) {
                ret = State::Win;
                nextNum = temp.nextNum;
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

        std::sort(nexts.begin(), nexts.end(), [](auto &lhs, auto &rhs) {
            return lhs.first > rhs.first;
        });

        for (const auto& next : nexts) {
            auto temp = dfs(otherNum, next.second);

            if (temp == State::Lose) {
                ret = State::Win;
                nextNum = next.second;
                winningCond = true;
                break;
            }
        }
    }

    if (winningCond) {
        auto sameNextNums = samePieces(nextNum);
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

    return ret;
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
