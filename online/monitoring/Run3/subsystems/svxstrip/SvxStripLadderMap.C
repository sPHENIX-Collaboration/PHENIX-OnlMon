#include "SvxStripLadderMap.h"

using namespace std;

SvxStripLadderMap::SvxStripLadderMap() :
    ladder_index_to_packetid(40)
{
    ladder_index_to_packetid[0] = 24101;
    ladder_index_to_packetid[1] = 24102;
    ladder_index_to_packetid[2] = 24103;
    ladder_index_to_packetid[3] = 24104;
    ladder_index_to_packetid[4] = 24108;
    ladder_index_to_packetid[5] = 24105;
    ladder_index_to_packetid[6] = 24106;
    ladder_index_to_packetid[7] = 24107;
    ladder_index_to_packetid[8] = 24117;
    ladder_index_to_packetid[9] = 24118;
    ladder_index_to_packetid[10] = 24119;
    ladder_index_to_packetid[11] = 24120;
    ladder_index_to_packetid[12] = 24121;
    ladder_index_to_packetid[13] = 24122;
    ladder_index_to_packetid[14] = 24123;
    ladder_index_to_packetid[15] = 24124;
    ladder_index_to_packetid[16] = 24109;
    ladder_index_to_packetid[17] = 24110;
    ladder_index_to_packetid[18] = 24111;
    ladder_index_to_packetid[19] = 24112;
    ladder_index_to_packetid[20] = 24113;
    ladder_index_to_packetid[21] = 24114;
    ladder_index_to_packetid[22] = 24115;
    ladder_index_to_packetid[23] = 24116;
    ladder_index_to_packetid[24] = 24125;
    ladder_index_to_packetid[25] = 24126;
    ladder_index_to_packetid[26] = 24127;
    ladder_index_to_packetid[27] = 24128;
    ladder_index_to_packetid[28] = 24129;
    ladder_index_to_packetid[29] = 24132;
    ladder_index_to_packetid[30] = 24130;
    ladder_index_to_packetid[31] = 24131;
    ladder_index_to_packetid[32] = 24133;
    ladder_index_to_packetid[33] = 24134;
    ladder_index_to_packetid[34] = 24135;
    ladder_index_to_packetid[35] = 24136;
    ladder_index_to_packetid[36] = 24137;
    ladder_index_to_packetid[37] = 24138;
    ladder_index_to_packetid[38] = 24139;
    ladder_index_to_packetid[39] = 24140;

    for (unsigned int i = 0; i < ladder_index_to_packetid.size(); i++)
        packetid_to_ladder_index.insert(make_pair(ladder_index_to_packetid[i], i));
}

int SvxStripLadderMap::get_ladder_index(int packet_index)
{
    if (packet_index < 24000) packet_index += 24101;
    return packetid_to_ladder_index[packet_index];
}

pair<int, int> SvxStripLadderMap::get_barrel_ladder(int packet_index)
{
    if (packet_index < 24000) packet_index += 24101;
    int iladder = get_ladder_index(packet_index);

    int barrel = (iladder < 16) ? 2 : 3;
    int ladder = (iladder < 16) ? iladder : (iladder - 16);
    return make_pair(barrel, ladder);
}

int SvxStripLadderMap::get_packetid(int barrel, int ladder)
{
    if (barrel < 2 || barrel > 3)
        return -1;

    if (barrel == 2 && (ladder < 0 || ladder > 15))
        return -1;

    if (barrel == 3 && (ladder < 0 || ladder > 23))
        return -1;

    int ladder_index = ladder;
    if (barrel == 3)
        ladder_index += 16;

    return ladder_index_to_packetid[ladder_index];
}