#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;

class unit_t {
  public:
    string start;
    string end;
    vector<int> days;
    vector<string> times; 
    string instructor;
    string component;
    string room;
};

class course_t {
  public:
    string title;
    vector<unit_t> components;
};

string convertTime(string t) {
  int h = atoi(t.substr(0, t.find(':')).c_str());
  if (t[t.size() - 2] == 'P') {
    if (h != 12)
      h += 12;
  }
  ostringstream os;
  os << h;
  string result = "";
  if (h < 10)
    result += '0';
  result += os.str();
  result += t.substr(t.find(':') + 1, 2) + "00";
  return result;
}

int leap(int y)
{
  if(y%400==0||(y%4==0&&y%100!=0))
    return 1;
  else
    return 0;
}

int numDays( int y, int m, int d )
{
  int sum = 0;
  switch(m) {
    case 1:sum=0;break;
    case 2:sum=31;break;
    case 3:sum=59;break;
    case 4:sum=90;break;
    case 5:sum=120;break;
    case 6:sum=151;break;
    case 7:sum=181;break;
    case 8:sum=212;break;
    case 9:sum=243;break;
    case 10:sum=273;break;
    case 11:sum=304;break;
    case 12:sum=334;break;
    default:break;
  }
  sum=sum+d;
  int leapyes=leap(y);
  if(leapyes==1 && m > 2) sum++;
  return sum;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    cerr << "no input file!" << endl;
    exit(1);
  }

  ifstream ifile;
  ifile.open(argv[1]);
  if(!ifile) {
    cerr << "cannot open file!" << endl;
    exit(1);
  }

  string line;
  vector<course_t> courses;
  getline(ifile, line);

  while(ifile) {
    if (line.find("PAGROUPDIVIDER") == string::npos) {
      getline(ifile, line);
      continue;
    }

    course_t tempC;
    while (line[0] == '<') {
      line.erase(0, line.find_first_of('>') + 1);
    }

    tempC.title = line.erase(line.find_first_of('<'));

    while (ifile) {
      do {
        getline(ifile, line);
      } while(line.find("MTG_COMP") == string::npos &&
              line.find("PAGROUPDIVIDER") == string::npos && ifile); 
      if (!ifile || line.find("PAGROUPDIVIDER") != string::npos)
        break;

      unit_t tempU;

      while (line[0] == '<') 
        line.erase(0, line.find_first_of('>') + 1);

      tempU.component = line.erase(line.find_first_of('<'));

      do {
        getline(ifile, line);
      } while(line.find("MTG_SCHED") == string::npos); 

      while (line[0] == '<') 
        line.erase(0, line.find_first_of('>') + 1);

      string sched;
      sched = line.erase(line.find_first_of('<'));
      while(sched[0] != ' ') {
        if (sched.substr(0, 2) == "Mo")
          tempU.days.push_back(1);
        else if (sched.substr(0, 2) == "Tu")
          tempU.days.push_back(2);
        else if (sched.substr(0, 2) == "We")
          tempU.days.push_back(3);
        else if (sched.substr(0, 2) == "Th")
          tempU.days.push_back(4);
        else if (sched.substr(0, 2) == "Fr")
          tempU.days.push_back(5);
        else if (sched.substr(0, 2) == "Sa")
          tempU.days.push_back(6);
        else if (sched.substr(0, 2) == "Su")
          tempU.days.push_back(0);

        sched.erase(0, 2);
      }
      sched.erase(0, 1);
      tempU.times.push_back(sched);

      do {
        getline(ifile, line);
      } while(line.find("MTG_LOC") == string::npos); 

      while (line[0] == '<') 
        line.erase(0, line.find_first_of('>') + 1);

      tempU.room = line.erase(line.find_first_of('<'));

      do {
        getline(ifile, line);
      } while(line.find("DERIVED_CLS_DTL_SSR_INSTR_LONG") == string::npos); 

      while (line[0] == '<') 
        line.erase(0, line.find_first_of('>') + 1);

      tempU.instructor = line.erase(line.find_first_of('<'));

      do {
        getline(ifile, line);
      } while(line.find("MTG_DATES") == string::npos); 

      while (line[0] == '<') 
        line.erase(0, line.find_first_of('>') + 1);

      tempU.start = line.substr(0, 10);
      tempU.end = line.substr(13, 10);

      tempC.components.push_back(tempU);

    }


    courses.push_back(tempC);

  }

  ifile.close();
  ofstream ofile;
  ofile.open("output.ics");

  ofile << "BEGIN:VCALENDAR\nPRODID:-//1859//iCal4j 1.0//EN\nVESION:1.0\nCALSCALE:1859\n"; 
  for (auto it = courses.begin(); it != courses.end(); ++it) {
    auto temp = it->components;
    for (unsigned i = 0; i < temp.size(); ++i) {
      for (unsigned j = 0; j < temp[i].days.size(); ++j) {
        auto tmp = temp[i];

        ofile << "BEGIN:VEVENT\nDTSTAMP:";

        string start, end;
        start = tmp.start.substr(6, 4) + tmp.start.substr(0, 2) + tmp.start.substr(3, 2); 
        end = tmp.end.substr(6, 4) + tmp.end.substr(0, 2) + tmp.end.substr(3, 2); 
        string st, et;
        st = tmp.times[0].substr(0, tmp.times[0].find_first_of(' '));
        et = tmp.times[0].substr(tmp.times[0].find_last_of(' ') + 1);
        st = convertTime(st);
        et = convertTime(et);

        time_t rawtime;
        struct tm * timeinfo;
        int year, mon, day;
        year = atoi(start.substr(0, 4).c_str());
        mon = atoi(start.substr(4, 2).c_str());
        day = atoi(start.substr(6, 2).c_str());
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        timeinfo->tm_year = year - 1900;
        timeinfo->tm_mon = mon - 1;
        timeinfo->tm_mday = day;

        mktime(timeinfo);
        int dif = tmp.days[j] - timeinfo->tm_wday;
        if (dif < 0) 
          dif += 7;

        day += dif;
        timeinfo->tm_mday = day;
        mktime(timeinfo);

        string tS;
        ostringstream os;
        os << timeinfo->tm_year + 1900;
        tS = os.str();
        start = tS;
        os.str("");
        os << timeinfo->tm_mon + 1;
        tS = os.str();
        cout << tS << endl;
        if (timeinfo->tm_mon < 10)
          start += '0';
        start += tS;
        os.str("");
        os << timeinfo->tm_mday;
        tS = os.str();
        if (timeinfo->tm_mday < 10)
          start += '0';
        start += tS;
        start += 'T';
        string endT = start;

        start += st;
        endT += et;

        ofile << start << "\nDTSTART:" << start << "\nDTEND:" << endT << "\nSUMMARY:" << it->title << ' ' << tmp.component << "\nLOCATION:" << tmp.room << "\nRRULE:FREQ=WEEKLY;COUNT=";

        int Eyear, Emon, Eday;
        Eyear = atoi(end.substr(0, 4).c_str());
        Emon = atoi(end.substr(4, 2).c_str());
        Eday = atoi(end.substr(6, 2).c_str());

        int d = numDays(Eyear, Emon, Eday) - numDays(year, mon, day);
        d = d/7 + 1;

        ofile << d << "\nEND:VEVENT\n";
      }
    }
  }
  ofile << "END:VCALENDAR\n";
  ofile.close();

  return 0;
}



