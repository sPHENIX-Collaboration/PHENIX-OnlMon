#ifndef __SVXSTRIPLADDERMAP_H__
#define __SVXSTRIPLADDERMAP_H__

#include <vector>
#include <map>

class SvxStripLadderMap
{
 public:
  SvxStripLadderMap();

  int get_ladder_index(int packet_index);
  std::pair<int,int> get_barrel_ladder(int packet_index);
  int get_packetid(int barrel, int ladder);

 private:
  std::vector<int> ladder_index_to_packetid;
  std::map<int,int> packetid_to_ladder_index;
};

#endif //__SVXSTRIPLADDERMAP_H__