#ifndef agari_hpp
#define agari_hpp

#include <vector>
#include <iostream>
#include <cstring>
#include <iostream>
#include "common.hpp"
#define sc static_cast

const std::string ManganName [] = {
    "满贯", "跳满", "倍满", "三倍满", "累计役满", 
    "役满", "两倍役满", "三倍役满", "四倍役满", "五倍役满", "六倍役满"
};

inline int Roundup10 (int a) { return (a + 9) / 10 * 10;}
inline int Roundup100 (int a) { return (a + 99) / 100 * 100;}

struct AgariPara {
    Wind SelfWind, PrevailingWind;
    int ReachTurn, AgariType, ReachCnt, Counters;
    bool isOneShot, isTenhou, isHaitei;
    Tile Target;
    std::vector <Tile> HandTile, Dora, UraDora;
    std::vector <Group> Groups;
    inline AgariPara () {}
    inline AgariPara (const Wind &selfwind, const Wind &prevailingwind, const int &agaritype, 
     const Tile &target,const std::vector <Tile> &handtile, const std::vector <Group> groups = NullGroups,
     const std::vector <Tile> &dora = NullTiles, const std::vector <Tile> &uradora = NullTiles,
     const int &reachturn = 0, const int &reachcnt = -1, const int &counters = 0,
     const bool &isoneshot = 0, const bool &istenhou = 0, const bool &ishaitei = 0)
     : SelfWind(selfwind), PrevailingWind(prevailingwind), AgariType(agaritype), Target(target),
     HandTile(handtile), Groups(groups), Dora(dora), UraDora(uradora), ReachTurn(reachturn), ReachCnt(reachcnt),
     Counters(counters), isOneShot(isoneshot), isTenhou(istenhou), isHaitei(ishaitei) {}
};

struct AgariResult {
    int AgariId, RonId, Counters;
    bool isEast, isTsumo, Reach;
    int Han, Fu;
    int Dora, AkaDora, UraDora;
    int PlainScore, AgariScore, RonScore, EastScore, OthersScore;
    std::vector <Yaku> yaku;
    inline AgariResult () {
        AgariId = RonId = Counters = isEast = isTsumo = Reach = Han = Fu = Dora = 
         AkaDora = UraDora = PlainScore = AgariScore = RonScore = EastScore = OthersScore = 0;
    }
    inline bool operator < (const AgariResult &rhs) const {
        return PlainScore < rhs.PlainScore;
    }
    inline bool operator == (const AgariResult &rhs) const {
        return PlainScore == rhs.PlainScore;
    }
    void GetScore (AgariPara para) {
        if (yaku.empty())
            return;
        std::sort(yaku.begin(), yaku.end());
        isTsumo = (para.AgariType == 0);
        isEast = (para.SelfWind == Wind::East);
        Reach = (para.ReachTurn > 0);
        Counters = para.Counters;
        int BasicPoint;
        if (Han < 0)
            BasicPoint = -8000 * Han;
        else if (Han >= 13)
            BasicPoint = 8000;
        else if (Han >= 11)
            BasicPoint = 6000;
        else if (Han >= 8)
            BasicPoint = 4000;
        else if (Han >= 6)
            BasicPoint = 3000;
        else if (Han >= 5)
            BasicPoint = 2000;
        else {
            BasicPoint = Fu * (1 << (Han + 2));
            if (BasicPoint > 2000)
                BasicPoint = 2000;
        }
        if (isEast) {
            if (isTsumo) {
                OthersScore = Roundup100(BasicPoint * 2);
                AgariScore = OthersScore * 3;
            } else {
                RonScore = Roundup100(BasicPoint * 6);
                AgariScore = RonScore;
            }
        } else {
            if (isTsumo) {
                EastScore = Roundup100(BasicPoint * 2);
                OthersScore = Roundup100(BasicPoint);
                AgariScore = EastScore + OthersScore * 2;
            } else {
                RonScore = Roundup100(BasicPoint * 4);
                AgariScore = RonScore;
            }
        }
        PlainScore = AgariScore;
        AgariScore += para.ReachCnt * 1000 + Counters * 300;
    }
};

enum struct AgariFailed {
    Null, WrongShape, NoYaku,
};

struct TryAgari {
    AgariResult Result;
    bool Success;
    AgariFailed Failed;
    inline TryAgari () : Failed(AgariFailed::Null), Success(false) {}
    inline TryAgari (const AgariResult &result) : Result(result), Success(true) {}
    inline TryAgari (const AgariFailed &failed) : Failed(failed), Success(false) {}
    void Print() {
        if (!Success) {
            std::cout << "Failed " << sc <int> (Failed) << std::endl;
            return;
        }
        if (Result.Han < 0)
            std::cout << ManganName[4 - Result.Han] << ' ';
        else {
            int ManganLevel = Result.PlainScore / 2000;
            if (Result.isEast)
                ManganLevel = ManganLevel * 2 / 3;
            ManganLevel >>= 1;
            if (ManganLevel > 2)
                std::cout << ManganName[ManganLevel >> 1] << ' ';
            else if (ManganLevel == 2)
                std::cout << ManganName[0] << ' ';
            if (Result.yaku[0] != Yaku::NagashiMangan)
                std::cout << Result.Han << "番" << Result.Fu << "符" << ' ';
        }
        if (Result.isTsumo) {
            if (Result.isEast)
                std::cout << Result.OthersScore << "All" << std::endl;
            else
                std::cout << Result.OthersScore << "-" << Result.EastScore << std::endl;
        }
        else
            std::cout << Result.PlainScore << std::endl;
        for (auto yaku : Result.yaku)
            std::cout << YakuName[sc <int> (yaku)] << std::endl;
        if (Result.Han > 0) {
            if (Result.Dora > 0)
                std::cout << "宝牌 " << Result.Dora << "番" << std::endl;
            if (Result.AkaDora > 0)
                std::cout << "红宝牌 " << Result.AkaDora << "番" << std::endl;
            if (Result.Reach)
                std::cout << "里宝牌 " << Result.UraDora << "番" << std::endl;
        }
    }
    void ProcessCounters () {
        if (Result.isTsumo) {
            Result.OthersScore += Result.Counters * 100;
            if (!Result.isEast)
                Result.EastScore += Result.Counters * 100;
        }
        else
            Result.RonScore += Result.Counters * 300;
        Result.Counters = 0;
    }
};

int cnt[34];

TryAgari isThirteenOrphans (AgariPara para) {
    memset(cnt, 0, sizeof cnt);
    AgariResult result;
    int YaochuuCnt = 0;
    for (auto handtile : para.HandTile) {
        int id = handtile.GeneralId;
        if (!cnt[id])
            cnt[id]++, YaochuuCnt++;
    }
    switch (YaochuuCnt) {
        case 13:
            if (!para.Target.isYaochuu())
                return TryAgari(AgariFailed::WrongShape);
            result.yaku.pb(Yaku::ThirteenOrphans13Wait);
            result.Han = -2;
            break;
        case 12:
            if (!para.Target.isYaochuu() || cnt[para.Target.GeneralId])
                return TryAgari(AgariFailed::WrongShape);
            result.yaku.pb(Yaku::ThirteenOrphans);
            result.Han = -1;
            break;
        default:
            return TryAgari(AgariFailed::WrongShape);
    }
    if (para.isTenhou) {
        result.Han -= 1;
        if (para.SelfWind == Wind::East) {
            result.yaku.pb(Yaku::Tenhou);
            if(result.yaku[0] == Yaku::ThirteenOrphans) {
                result.yaku[0] = Yaku::ThirteenOrphans13Wait;
                result.Han -= 1;
            }
        } else
            result.yaku.pb(Yaku::Chiihou);
    }
    result.GetScore(para);
    return TryAgari(result);
}

TryAgari isSevenPairs (AgariPara para) {
    memset(cnt, 0, sizeof cnt);
    AgariResult result;
    int PairCnt = 0;
    for (auto tile : para.HandTile)
        if (++cnt[tile.GeneralId] == 2)
            PairCnt++;
    if (++cnt[para.Target.GeneralId] == 2)
        PairCnt++;
    if (PairCnt < 7)
        return TryAgari(AgariFailed::WrongShape);
    if (para.isTenhou) {
        result.Han = -1;
        if (para.SelfWind == Wind::East)
            result.yaku.pb(Yaku::Tenhou);
        else
            result.yaku.pb(Yaku::Chiihou);
    }
    bool isAH = 1;
    for (int i = 0; i < 27; i++)
        if (cnt[i]) {
            isAH = 0;
            break;
        }
    if (isAH) {
        result.Han -= 1;
        result.yaku.pb(Yaku::AllHonors);
    }
    if (!result.Han) {
        result.yaku.pb(Yaku::SevenPairs);
        result.Han += 2;
        result.Fu = 25;
        if (para.ReachTurn > 0) {
            if (para.ReachTurn == 1) {
                result.yaku.pb(Yaku::DoubleRiichi); 
                result.Han += 2;
            } else {
                result.yaku.pb(Yaku::Riichi); 
                result.Han += 1;
            }
            if (para.isOneShot) {
                result.yaku.pb(Yaku::Ippatsu);
                result.Han += 1;
            }
        }
        if (!para.AgariType) {
            result.yaku.pb(Yaku::FullyConcealedHand); 
            result.Han += 1;
        }
        if (para.isHaitei) {
            result.Han += 1;
            if (para.AgariType)
                result.yaku.pb(Yaku::Houtei);
            else
                result.yaku.pb(Yaku::Haitei);
        }
        bool isAS = 1, isATH = 1;
        for (int i = 0; i < 34; i++)
            if (cnt[i]) {
                if (isYaochuu[i])
                    isAS = 0;
                else
                    isATH = 0;
            }
        if (isAS) {
            result.yaku.pb(Yaku::AllSimples);
            result.Han += 1;
        }
        if (isATH) {
            result.yaku.pb(Yaku::AllTerminalsAndHonors);
            result.Han += 2;
        }
        int col = (sc <int> (para.HandTile[0].Color) - 109) / 3;
        int colcnt = 0;
        for (int i = 0; i < 9; i++)
            if (cnt[col * 9 + i])
                colcnt++;
        if (colcnt == 7) {
            result.yaku.pb(Yaku::FullFlushClosed);
            result.Han += 6;
        } else {
            for (int i = 27; i < 34; i++)
                if (cnt[i])
                    colcnt++;
            if (colcnt == 7) {
                result.yaku.pb(Yaku::HalfFlushClosed);
                result.Han += 3;
            }
        }
        for (auto dora : para.Dora)
            if (cnt[dora.Next().GeneralId])
                result.Dora += 2;
        for (auto tile : para.HandTile)
            if (tile.isAka)
                result.AkaDora++;
        if (para.Target.isAka)
            result.AkaDora++;
        for (auto uradora : para.UraDora)
            if (cnt[uradora.Next().GeneralId])
                result.UraDora += 2;
        result.Han += result.Dora + result.AkaDora + result.UraDora;
    }
    result.GetScore(para);
    return TryAgari(result);
}

TryAgari Agari (AgariPara para) {
    std::sort(para.HandTile.begin(), para.HandTile.end());
    TryAgari TO = isThirteenOrphans(para);
    if (TO.Success)
        return TO;
    TryAgari SP = isSevenPairs(para);
    if (SP.Success)
        return SP;
    return TryAgari();
    memset(cnt, 0, sizeof cnt);
    for (auto handtile : para.HandTile)
        cnt[handtile.GeneralId]++;
    for (auto groups : para.Groups) {
        auto tiles = groups.getTiles();
        for (auto tile : tiles)
            cnt[tile.GeneralId]++;
    }
}

#undef sc

#endif // agari_hpp