#include <timeHist.h>
#include <muiMonGlobals.h>
#include <OnlMonServer.h>
#include <TH2.h>

timeHist::timeHist(OnlMon *myparent): obj_hist(myparent)
{
  OnlMonServer *Onlmonserver = OnlMonServer::instance();
  clear_overflow = 0;

  total_hits_timedep = new TH2F("MUI_TotalHits_TimeDep", "MuID total number of hits Time dependence", max_timebin, -0.5, max_timebin - 0.5, 100, 0, 100); //for p+p events
  Onlmonserver->registerHisto(parent,total_hits_timedep);
}

void timeHist::event(hit_vector rawHits)
{
  muiMonGlobals* globals = muiMonGlobals::Instance();
  static int timebin = 0;
  static int hits_accumulate = 0;

  if ( timebin > max_timebin )
    {
      if ( clear_overflow )
        {
          timebin = 0;
          hits_accumulate = 0;
          clear();
        }
    }
  else
    {
      hits_accumulate += rawHits.size();
      //      cout <<"Accumulated hits "<< hits_accumulate<<endl;
      if ( (globals->eventNum > 0) && ( globals->eventNum % events_accumulate == 0) )
        {
          total_hits_timedep->Fill( timebin, (float)hits_accumulate / events_accumulate );
          //        cout<<" Average hits in "<<events_accumulate<<" events: "<<(float)hits_accumulate/events_accumulate<<endl;
          timebin++;
          hits_accumulate = 0;
        }
    }
}

void timeHist::clear()
{
  total_hits_timedep->Reset();
}

void timeHist::draw()
{
}



