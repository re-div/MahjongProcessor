#ifndef yaku_hpp
#define yaku_hpp

#define sc static_cast

/*
0:一番
1:一番，门前清限定
3:二番
4:二番，门前清限定
5:二番，副露减一番
7:三番，门前清限定
8:三番，副露减一番
9:满贯
a:六番，副露减一番
b:役满
c:役满，门前清限定
e:两倍役满
f:两倍役满，门前清限定
*/
enum struct Yaku {
    Reach = 0x10,
    SevenPairs = 0x40,
    NagashiMangan = 0x90,

    SelfPick = 0x11,
    OneShot = 0x12,
    Haitei = 0x01,
    Houtei = 0x02,
    Rinshan = 0x03,
    Chankan = 0x04,
    DoubleReach = 0x41,

    Pinfu = 0x17,
    Iipeikou = 0x18,
    Sanshoku = 0x50,
    Straight = 0x51,
    Ryanpeiko = 0x70,

    Toitoi = 0x30,
    Sanankou = 0x31,
    SanshokuDoukou = 0x32,
    ThreeKans = 0x33,

    Tanyao = 0x05,
    HonorTiles_White = 0x06,
    HonorTiles_Green = 0x07,
    HonorTiles_Red = 0x08,
    HonorTiles_PlayerWind = 0x09,
    HonorTiles_PrevailingWind = 0x0a,
    Chanta = 0x52,
    Junchan = 0x80,
    Honrou = 0x34,
    LittleThreeDragons = 0x35,

    HalfFlush = 0x81,
    FullFlush = 0xa0,

    ThirteenOrphans = 0xc0,
    ThirteenOrphans13Wait = 0xf0,
    Suuankou = 0xc1,
    SuuankouTanki = 0xf1,
    BigThreeDrangons = 0xb0,
    SmallFourWinds = 0xb1,
    BigFourWinds = 0xe0,
    AllHonors = 0xb2,
    AllTerminals = 0xb3,
    AllGreen = 0xb4,
    NineGates = 0xc2,
    NineGates9Wait = 0xf2,
    FourKans = 0xb5,
    Tenhou = 0xc3,
    Chiihou = 0xc4,
};

int ProcessHan (const Yaku &Num, const bool& isOpen) {
    int Han = (sc <int> (Num) & 0xf0) >> 4;
    if (Han < 9)
        switch (Han % 3) {
            case 0:
                return Han / 3 + 1;
            case 1:
                return isOpen ? 0 : Han / 3 + 1;
            case 2:
                return isOpen ? Han / 3 : Han / 3 + 1;
        }
    else
        switch (Han) {
            case 9:
                return 5;
            case 10:
                return isOpen ? 5 : 6;
            case 11: case 14:
                return - ((Han - 8) / 3);
            case 12: case 15:
                return isOpen ? 0 : - ((Han - 8) / 3);
        }
}

#undef sc

#endif // yaku_hpp