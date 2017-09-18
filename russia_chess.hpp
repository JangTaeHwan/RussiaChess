#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <list>
#include <map>
#include <cmath>

constexpr int positionSize = 100000;
constexpr int pointPow = 46;

constexpr int pieceSize = 6;
constexpr int mapSize = 9;

enum class State {
    Notvalid = 0,
    Processing,
    Normal,
    Win,
    Lose,
};

struct TurnState {
    int nextNum = 0;
    short priority = 0;
    State state = State::Normal;
};

struct Move {
    int target;
    int point;
};

using StateMap = std::map<int, State>;
using NextMap = std::map<int, int>;
using NextInfo = std::pair<short, int>; // priority, nextNum
using Container = std::vector<short>;
using Point = std::pair<short, short>;

const std::vector<Point> bingoSet[10] =
{
{},
{{2,3},{5,9},{4,7}},
{{1,3},{5,8}},
{{1,2},{5,7},{6,9}},
{{1,7},{5,6}},
{{1,9},{2,8},{3,7},{4,6}},
{{3,9},{4,5}},
{{1,4},{3,5},{8,9}},
{{2,5},{7,9}},
{{1,5},{3,6},{7,8}},
};

const std::vector<Container> sameMapSet =
{
{ 0,1,2,3,4,5,6,7,8,9 },
{ 0,7,4,1,8,5,2,9,6,3 },
{ 0,9,8,7,6,5,4,3,2,1 },
{ 0,3,6,9,2,5,8,1,4,7 },
{ 0,7,8,9,4,5,6,1,2,3 },
{ 0,1,4,7,2,5,8,3,6,9 },
{ 0,3,2,1,6,5,4,9,8,7 },
{ 0,9,6,3,8,5,2,7,4,1 },
};

const std::vector<int> targetPriority =
{ 0,4,5,1,2,3 };

const std::vector<int> pointPriority =
{ 5,1,3,7,9,2,4,8,6 };

// 현재 턴일때 승부결과와 다음 수 저장
StateMap stateMap[positionSize];  // stateMap[ourNum][otherNum] = state;
NextMap nextMap[positionSize];    // nextMap[ourNum][otherNum] = nextNum;

std::vector<Point> intToPoint(60);
int pointToInt[10][10];    //  [min][max];

inline void initPointDict() {
    int count = 0;
    intToPoint[count] = {0, 0};
    pointToInt[0][0] = count;

    for(auto i=0; i<10; i++) {
        for(auto j=i+1; j<10; j++) {
            intToPoint[++count] = { i, j };
            pointToInt[i][j] = count;
        }
    }
}

inline int encodePieces(const Container& pieces) {
    int ret = 0;
    for (int i=2; i>=0; i--) {
        int a = std::min(pieces[i*2], pieces[i*2 + 1]);
        int b = std::max(pieces[i*2], pieces[i*2 + 1]);
        ret *= pointPow;
        ret += pointToInt[a][b];
    }
    return ret;
}

inline auto decodeNum(int num) {
    Container ret;
    for (int i=0; i<3; i++) {
        int temp = num % pointPow;
        const auto& point = intToPoint[temp];

        ret.push_back(point.first);
        ret.push_back(point.second);
        num /= pointPow;
    }
    return ret;
}

inline bool checkValid(
    const Container& ours,
    const Container& others,
    const Move& m)
{
    int targetNum = ours[m.target] > 0 ? ours[m.target] : -1;
    int start = m.target % 2 == 1 ? m.target : m.target - 1;
    for (int i=start; i<pieceSize; i++) {
        if (ours[i] == m.point
            || others[i] == m.point
            || others[i] == targetNum) {
            return false;
        }
    }
    return true;
}

inline bool checkWinning(
    const Container& ours,
    const Container& others,
    const Move& m,
    short& priority)
{
    Container map(10, 0);
    for(int i=0; i<pieceSize; i++) {
        map[ours[i]] = 1;
        map[others[i]] = 2;

        if (ours[i] > 0)
            priority++;
    }

    for (const auto& points : bingoSet[m.point]) {
        if (map[points.first] == 1
            && map[points.second] == 1)
            return true;
    }
    return false;
}

inline auto samePieces(const Container& pieces) {
    std::vector<int> ret;

    for (const auto& set : sameMapSet) {
        Container samePiece(pieceSize);

        for (int i=0; i<pieceSize; i++) {
           samePiece[i] = set[pieces[i]];
        }
        ret.push_back(encodePieces(samePiece));
    }

    return ret;
}

inline auto samePieces(int num) {
    const Container pieces = decodeNum(num);

    return samePieces(pieces);
}
