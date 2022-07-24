#ifndef __BBCMSGTYPES_H__
#define __BBCMSGTYPES_H__

// the number of messages which are send into the message system is
// progressively throttled to not overwhelm it. The first 10 messages
// are send, then only every 2nd,4th,8th etc. Messages which have
// the same type (for lack of a better word) are throttled together

enum {
  NOBBCPKT = 1,
  NOBBCLL1PKT = 1,
  VTXDIFF = 2,
  TIMEDIFF = 3,
  TIMESOUTH = 4,
  MULTSOUTH = 5,
  MULTNORTH = 6,
  VERTEXOK = 7,
  REDBITS = 8,
  NUMWARN = 9,
  NUMFATAL = 9,
  FILEOPEN = 10,
  LUTFORMAT = 11
};

#endif
