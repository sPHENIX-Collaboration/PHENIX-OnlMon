#ifndef __MUI_HIT_H__
#define __MUI_HIT_H__
#include <vector>

///Contains all information about a muid hit.
struct mui_hit
  {
    short fem;
    short roc;
    short word;
    short bit;
    short arm;
    short plane;
    short panel;
    short orient;
    short twopack;
  };

///A vector collection of muid hits
typedef std::vector<mui_hit> hit_vector;

///An iterator for a hit_vector
typedef hit_vector::iterator hit_iter;

#endif /* __MUI_HIT_H__ */
