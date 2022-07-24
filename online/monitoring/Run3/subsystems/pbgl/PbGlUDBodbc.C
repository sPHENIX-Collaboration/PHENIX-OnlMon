#include <PbGlUDBodbc.h>
#include <phool.h>
#include <odbc++/connection.h>
#include <odbc++/setup.h>
#include <odbc++/types.h>
#include <odbc++/errorhandler.h>
#include <sql.h>
#include <odbc++/drivermanager.h>
#include <odbc++/resultset.h>
#include <odbc++/resultsetmetadata.h>
#include <iostream>

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <set>
#include <sstream>

using namespace odbc;
using namespace std;

static Connection *con = 0;

PbGlUDBodbc::PbGlUDBodbc(const string &name): OnlMonBase(name)
{
  dbname = "OnlMonDB";
  dbowner = "phnxrc";
  table = name;
// The bizarre cast here is needed for newer gccs
  transform(table.begin(), table.end(), table.begin(), (int(*)(int))tolower);
  dbpasswd = "";
  tableexist = 0;
}

PbGlUDBodbc::~PbGlUDBodbc()
{
  if (con)
    {
      delete con;
      con = 0;
    }
}

int
PbGlUDBodbc::CheckAndCreateTable()
{
  if (GetConnection())
    {
      return -1;
    }
  if (tableexist)
    {
      return 0;
    }
  //Postgres version
  //cout << con->getMetaData()-> getDatabaseProductVersion() << endl;
  Statement* stmt = con->createStatement();
  ostringstream cmd;
  cmd << "select * from pg_tables where tablename = '" << table << "'";
  //  cmd << "SELECT * FROM " << table << " LIMIT 1" ;
  if (verbosity > 0)
    {
      cout << "cmd: " << cmd.str() << endl;
    }

  ResultSet *rs = 0;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << table << " does not exist, creating it" << endl;
    }
  int iret = 0;

  cmd.str("");
  if (! rs->next())
    {
      delete rs;
      rs = 0;
      cmd << "CREATE TABLE " << table << " (runnumber int NOT NULL, time bigint, packet int NOT NULL, what text, primary key(runnumber,time,packet,what))";
      if (verbosity > 0)
        {
          cout << "Executing " << cmd.str() << endl;
        }
      try
        {
          iret = stmt->executeUpdate(cmd.str());
        }
      catch (SQLException& e)
        {
          cout << "Exception caught, Message: " << e.getMessage() << endl;
        }
    }
  delete stmt;
  tableexist = 1;
  return iret;
}


int
PbGlUDBodbc::AddRow(const int runnumber, const time_t unixtime, const int packetid, const string &what)
{
  if (CheckAndCreateTable() < 0)
    {
      return  -1;
  }


  if (GetConnection())
    {
      return -1;
    }

  int iret = 0;
  ostringstream cmd;


  Statement* stmt = con->createStatement();

  // check if an entry for this run exists already
  cmd << "SELECT what FROM " << table << " where runnumber = "
      << runnumber << " and packet = " << packetid << " and time = " << unixtime ;
  ResultSet *rs;
  try
    {
      rs = stmt->executeQuery(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, Message: " << e.getMessage() << endl;
      return -1;
    }
  if (rs->next())
    {
      string wh = rs->getString("what");
      printf("Entry for run %d, packet %d, unixtime %lu exists with what= %s\n",
	     runnumber, packetid, unixtime, wh.c_str());
      delete rs;
      return 0;
    }
  delete rs;


  cmd.str("");
  cmd << "INSERT INTO " << table
      << " (runnumber, time, packet, what) VALUES(" << runnumber << ", " << unixtime << ", " << packetid << ", '" << what << "')";
  try
    {
      stmt->executeUpdate(cmd.str());
    }
  catch (SQLException& e)
    {
      cout << "Exception caught, Message: " << e.getMessage() << endl;
    }
  delete stmt;
  return iret;
}


int
PbGlUDBodbc::GetConnection()
{
  if (con)
    {
      return 0;
    }
  try
    {
      con = DriverManager::getConnection(dbname.c_str(), dbowner.c_str(), dbpasswd.c_str());
    }
  catch (SQLException& e)
    {
      cout << PHWHERE
	   << " Exception caught during DriverManager::getConnection" << endl;
      cout << "Message: " << e.getMessage() << endl;
      if (con)
        {
          delete con;
          con = 0;
        }
      return -1;
    }
  printf("opened DB connection\n");
  return 0;
}

