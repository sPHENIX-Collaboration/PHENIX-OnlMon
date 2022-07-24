#ifndef RICHCAL_H_INCLUDED
#define RICHCAL_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "PHTimeStamp.h"
#include "PdbBankManager.hh"
#include "PdbApplication.hh"
#include "PdbADCChan.hh"
#include "PdbCalBank.hh"
#include "PdbBankList.hh"
#include "PdbBankListIterator.hh"
#include "PdbBankID.hh"


template <class T>
class CrkCal {
  //
  // class for database and file I/O of
  // ADC calibration constants of RICH detector.
  //
  //  read_file   ... read calib. constants from ASCII text file and put
  //                  them in memory
  //  fetch_DB    ... fetch calib. constants. from database and put them
  //                  in memory
  //  store_DB    ... store calib. constants in memory to database
  //
  // The data in database is tagged by TimeStamp and BankID.

private: // data members
  int                d_status;     //status
  std::string             d_name;       //name of this calbration data

  std::string             d_classname;  //class name used in pdbcal
  std::string             d_calibname;  //calib name (path to DB file) in pdbcal
  std::vector<T>     d_calib;      //calibration data
  PdbBankID          d_bankID;     //bank ID used in pdbcal
  std::string             d_description;//description stored in pdbcal
  PHTimeStamp        d_start_time; //start of valid time range in pdbcal
  PHTimeStamp        d_end_time;   //end of valid time range in pdbcal

private: // helper functions
  static void read_time(std::istream & is, PHTimeStamp &time);
  static bool skip_comment(std::ifstream&);
  void write_header(std::ostream&);

public:
  CrkCal(const char *);

  void print(int level=0);
  bool good(void) {return d_status == 1;}
  int  size(void) {return d_calib.size();}
  void resize(int size) {d_calib.resize(size);}
  void setval(int i, T value) {d_calib[i]=value;}
  void getval(int i, T& value) {value = d_calib[i];}

  void read_file(void);
  void read_file(const std::string &fname);
  void write_file(void);
  void write_file(const std::string &fname);
  void fetch_DB(void);
  void fetch_DB(PHTimeStamp &SearchTime);
  void store_DB(void);
};

//
// Because of a limitation of g++, one can not separate compilation of
// a template class. All memeber function should be defined in the
// header file.
//

template <class T>
bool CrkCal<T>::skip_comment(ifstream& ifile) {
  char c;

  while(! ifile.eof()) {
    ifile.get(c);
    if( c != '#') {
      ifile.unget();
      break;
    }
    ifile.ignore(100000,'\n');
  }

  if(ifile.eof() || !ifile.good()) return false;
  else return true;
}

template <class T>
void CrkCal<T>::read_time(istream& is, PHTimeStamp &time) {
  char c;
  int year, month, date, hour, minute, second;

  is.get(c);
  is.unget();
  if( isdigit(c)) {
    is >> year >> month >> date >> hour >> minute >> second;
    if(!is.good()) return;
    time.set(year,month,date,hour,minute,second);
  } else {
    is >> time;
  }
  is.ignore(1000,'\n');
}

template <class T>
void CrkCal<T>::print(int level) {
  if(d_calib.size() == 0) {
    cout << "No data " << endl;
    return;
  }
  if( level < 0 || level > 2) {
    cout << "level should be 1 or 2 or 3" << endl;
    cout << "0:  print header only"<<endl;
    cout << "1:  print header and all entry"<<endl;
    cout << "2:  print header and a range of entry"<<endl;
    return;
  }

  cout << "Class name : " << d_classname << endl;
  cout << "calibname  : " << d_calibname << endl;
  cout << "BankID     : " << d_bankID.getInternalValue() << endl;
  cout << "StartTime  : " << d_start_time << endl;
  cout << "EndTime    : " << d_end_time << endl;
  cout << "Description: " << d_description << endl;
  cout << "Data Size  : " << d_calib.size() << endl;

  int imin, imax;

  if(level == 0) {  // print header only
    imin = 0;
    imax = 0;
  } else if(level == 1) { // print all data
    imin = 0;
    imax = d_calib.size();
  } else if(level == 2) { // print selected range
    cout << "Enter range: [imin, imax]"<<endl;
    cout << " imin? ";
    cin >> imin;
    cout << " imax? ";
    cin >> imax;
    if( imin < 0 ) imin = 0;
    if( imax > d_calib.size()) imax = d_calib.size();
  }

  int linecounts = 0;
  for(int i=imin; i< imax; i++) {
    cout << i << ") ";
    d_calib[i].print();
    ++linecounts;
    if(linecounts > 20) {
      linecounts = 0;
      //      d_calib[0].print_field();
      char answer;
      cout << "countine (Y/N)? ";
      cin >> answer;
      if(answer == 'N' || answer == 'n') break;
    }
  }
}


template <class T>
void CrkCal<T>::read_file(void) {
  std::string fname;

  cout << " Enter file name of input calibration data: ";
  cin >> fname;
  read_file(fname);
}

template <class T>
void CrkCal<T>::read_file(const std::string &fname) {
//
// Read calibration data from a text file.
// ';' at the beginning of a line indicates a comment line (ignored)
// The format of the input file is
// bankID
// year month date hour minute second (of start_time)
// year month date hour minute second (of end_time)
// description
// data
//
//
  cout << " read data from " << fname << endl;
  d_status = -1;

  ifstream ifile(fname.c_str());
  if( !ifile.is_open()) {
    cout << "Cannot open file: "<<fname << endl;
    return;
  }

// read name of the data and compare it with d_name
  std::string name;
  if(!skip_comment(ifile)) return;
  ifile >> name;
  if(!ifile.good()) return;

  if(name != d_name) {
    cout << "Wrong name in data file. Name should be "<<d_name<<endl;
    return;
  }
  ifile.ignore(1000,'\n');

// read d_start_time  
  if(!skip_comment(ifile)) return;
  read_time(ifile, d_start_time);
  //  cout << "start_time = "<<d_start_time<<endl;

// read d_end_time
  if(!skip_comment(ifile)) return;
  read_time(ifile, d_end_time);
  //  cout << "end_time = "<<d_end_time<<endl;

// read d_description
  if( !skip_comment(ifile)) return;
  char linebuf[200];
  ifile.getline(linebuf,sizeof(linebuf));
  d_description = linebuf;
  //  cout << "descriptoin:"<<d_description<<endl;
  
// read calibration data from the file
  d_calib.resize(0);
  while(!ifile.eof()) {
    skip_comment(ifile);
    if(ifile.eof()) break;
    T data;
    data.read(ifile);
    d_calib.push_back(data);
  }
  //  cout << d_calib.size() << " entried read"<<endl;
  d_status = 1;
}

template <class T>
void CrkCal<T>::write_file(void) {
  std::string fname;

  cout << " Enter file name to store the calibration data: ";
  cin >> fname;
  write_file(fname);
}

template <class T>
void CrkCal<T>::write_file(const std::string &fname) {
  ofstream ofile(fname.c_str());

  ofile << "# Name (bankID="<<d_bankID.getInternalValue()<<")" << endl;
  ofile << d_name<<endl;
  ofile << "# start time "<<endl;
  ofile << d_start_time << endl;
  ofile << "# end time "<<endl;
  ofile << d_end_time << endl;
  ofile << "# description "<<endl;
  ofile << d_description << endl;
  ofile << "# calibration data. " << d_calib.size() << " entries"<<endl;
  write_header(ofile);
  for(int i=0;i<d_calib.size();i++) {
    d_calib[i].write(ofile);
  }
}

template <class T>
void CrkCal<T>::store_DB(void) {
  cout << "store_DB is called" << endl;

  PdbBankManager *bkMngr = PdbBankManager::instance();
  PdbApplication *app = bkMngr->getApplication();

  if(app->startUpdate()) {
    //
    // If there is a bank already in the DB that has overlap of valid
    // time range with this calibration data, its EndValTime is set to
    // the StartValTime of this calibration.
    //
    cout << "bankID = "<<d_bankID.getInternalValue()<<endl;
    PdbCalBank *prev_bank =
      bkMngr->fetchBank(d_classname.c_str(), d_bankID,
			d_calibname.c_str(), d_start_time);
    if(prev_bank) {
      cout << " overlapping bank found. Change the EndValTime of it " <<endl;
      prev_bank->setEndValTime(d_start_time);
    }
    //
    // Now create a new bank in DB for this calibration, and store the
    // calibration data in the bank 
    //
    PdbCalBank *richBank = 
      bkMngr->createBank(d_classname.c_str(), d_bankID, d_description.c_str(),
			 d_start_time,d_end_time,d_calibname.c_str());
    richBank->setLength(d_calib.size());

    for(int i=0;i<d_calib.size();i++) {
      T *entry = (T*)&(richBank->getEntry(i));
      *entry = d_calib[i];
    }

    //
    // Commit the both changes in the DB
    //
    app->commit();
  } else {
    cout << "failed to start application for update" << endl;
  }
}

template <class T>
void CrkCal<T>::fetch_DB(void) {
  int year, month, day, hour, minute, sec, bankID;

  cout << " Enter search_time (year, month, day, hour, minute, sec)"<<endl;
  cin >> year >> month >> day >> hour >> minute >> sec;
  PHTimeStamp search_time(year, month, day, hour, minute, sec);

  cout << "Look up for Time = " << search_time;

  fetch_DB(search_time);
}

template <class T>
void CrkCal<T>::fetch_DB(PHTimeStamp &search_time) {

  PdbBankManager *bkMngr = PdbBankManager::instance();
  PdbApplication *app = bkMngr->getApplication();

  if( app->startRead()) {
    PdbCalBank *richBank = 
      bkMngr->fetchBank(d_classname.c_str(), d_bankID,
			d_calibname.c_str(), search_time);
    if( richBank ) {
      //      richBank->printHeader();
      d_start_time = richBank->getStartValTime();
      d_end_time   = richBank->getEndValTime();
      d_description = richBank->getDescription().getString();
      d_calib.resize(richBank->getLength());
      for (unsigned int i=0; i < richBank->getLength(); i++) {
	d_calib[i] = (T &) richBank->getEntry(i);
      }
      //      cout << "Data size = " << richBank->getLength() << endl;
      d_status = 1;
    } else {
      cout << " bank not found " << endl;
      d_status = -1;
    }
    app->commit();
  } else {
    app->abort();
    cout << "Transaction aborted." << endl;
    d_status = -1;
  }
}

#endif









