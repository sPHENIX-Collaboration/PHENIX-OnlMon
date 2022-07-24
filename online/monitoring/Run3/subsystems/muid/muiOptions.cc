#include <muiOptions.h>
#include <iostream>
#include <msg_profile.h>
#include <msg_control.h>
#include <muiMonGlobals.h>

#include <phool.h>

#include <sstream>

using namespace std;

muiOptions::muiOptions()
{
  verbose = muiMonGlobals::Instance()->verbose;
  auto_histo_clear = true;
  fill_from_db = true;
  auto_histo_save = 0;
  livetrig_accept = -1; // take all as default
  max_mult = 300;
  min_mult = 0;
}

muiOptions::~muiOptions()
{}

void muiOptions::setVerbose(int verb)
{
  verbose = verb;
  muiMonGlobals::Instance()->verbose = verb;
}
int muiOptions::getVerbose()
{
  return verbose;
}

void muiOptions::setArmActive(int arm, bool b)
{
  arm_active[arm] = b;
  muiMonGlobals::Instance()->arm_active[arm] = b;
}
bool muiOptions::getArmActive(int arm)
{
  return arm_active[arm];
}

void muiOptions::setAutoClearHists(bool aut)
{
  auto_histo_clear = aut;
}
bool muiOptions::getAutoClearHists()
{
  return auto_histo_clear;
}

void muiOptions::set_trigger(long trigBits)
{
  livetrig_accept = trigBits;
}

void muiOptions::set_trigger()
{
  ostringstream msg;
  msg << " Be aware that hardwired trigger bit info is"
      << " bound to be obsolete soon enough../DS 2004-11-17" << endl
      << "A choice of -1 gives you all triggers, 0 gives you minbias and"
      << endl << " for other values a bitmask is done " << endl;
  msg_control *muimsg = new msg_control(MSG_TYPE_MONITORING,
					MSG_SOURCE_MUID,
					MSG_SEV_ERROR, "MUID muiOptions");
  cout << *muimsg << PHWHERE << msg << endl;
  delete muimsg;
}

long muiOptions::get_trigger()
{
  return livetrig_accept;
}

void muiOptions::auto_save(short i)
{
  auto_histo_save = i;
}

void muiOptions::setUseDB(bool useDB)
{
  fill_from_db = useDB;
}

bool muiOptions::useDB()
{
  return fill_from_db;
}
