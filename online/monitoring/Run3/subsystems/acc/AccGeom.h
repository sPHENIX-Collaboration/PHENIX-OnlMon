#ifndef __ACCGEOM__
#define __ACCGEOM__

class AccGeom {

 public:
  AccGeom() {}
  virtual ~AccGeom() {}
  static int get_pmtnum(int ich,int fem_id);
  int get_fem_address(int pmtnum,int *fem_id,int *chan);
  static int get_pmt_coords(int pmtnum,int *sect);
  void get_boxid(int pmtnum, int *boxid);
  void get_pmtid(int boxid, int *pmtleft, int *pmtright);
};


#endif /* __ACCGEOM__ */








