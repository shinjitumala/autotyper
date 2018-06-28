/**
 * Paycheck2.cpp
 *
 * ---------------
 *
 *
 */

#include <windows.h>
#include <iostream>
#include <queue>
#include <chrono>
#include <ctime>
#include <cstdlib>
using namespace std;

bool debug;

/**
 * Keeps a string queue to be send to a specific window.
 * Will send all queued strings in one operation when sendToWindow() method is called.
 */
class MessageSender{
  double inputDelay; // the delay between inputs. in milliseconds.
  string windowName; // the window name to send the input to. the window that contains this name will be selected.
  int MaxWindows; // the maximum number of windows that the ActivateWindow() method will look through. This is to avoid infinite alt tabbing.
  queue<string> messageQueue; // message queue to send multiple messages in one alt tab operation.


public:
  /**
   * Constructor
   */
  MessageSender(int delay, int maxW, string WName){
    inputDelay = delay; // in milliseconds
    MaxWindows = maxW;
    windowName = WName;
  }

  /**
   * Queues a string to be sent to the window in the next operation.
   * string s: the string that is to be queued. (*WARNING*: read method sendString() for details.)
   */
  void queueMessage(string s){
    messageQueue.push(s);
  }

  /**
   * Attempts to send strings that are queued as keyboard input to the window that contains "windowName"
   * in its title. Will attempt change the window to paycheck.exe after.
   * bool return value: returns false if failed. otherwise returns true
   */
  bool sendToWindow(){
    if(messageQueue.empty()) return true; // do nothing if queue is empty.
    bool ret = ActivateWindow(windowName);
    if(ret){ // if success.
      while(!messageQueue.empty()){
        Sleep(inputDelay);
        sendString(messageQueue.front());
        messageQueue.pop();
      }
      Sleep(inputDelay);
    }
    ActivateWindow("paycheck.exe");
    return ret;
  }

private:
  /**
   * Sends a string as keyboard input.
   * *WARNING*: It's incomplete.
   * string s: The string to be send as keyboard input. (Can only accpets a handful of inputs.)
   */
  void sendString(string s){
    for(unsigned int i = 0; i < s.length() + 1; i++){
      if('a' <= s[i] && 'z' >= s[i]){
        sendKey(s[i] - 32, false);
      }else if('A' <= s[i] && 'Z' >= s[i]){
        sendKey(s[i], true);
      }else if(s[i] == '\n'){ // '\n' acts as enter key.
        sendKey(13, false);
      }else if('$' == s[i]){
        sendKey(52, true);
      }else if('-' == s[i]){
        sendKey(109, false);
      }else if('0' <= s[i] && '9' >= s[i]){
        sendKey(s[i], false);
      }else if('\0' == s[i]){
        // do nothing.
      }else{
        cout << "MessageSender() >> invalid character: " << s[i] <<". skipped." << endl;
      }

      Sleep(inputDelay);
    }
  }

  /**
   * Sends a keypress.
   * unsigned char code: The code for the simulted keypress.
   * bool shift: Weather the key is pressed with the shift key or not.
   */
  void sendKey(unsigned char code, bool shift){
    if(shift){
      keybd_event(160, 0x00, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(code, 0x00, KEYEVENTF_EXTENDEDKEY | 0, 0);
      Sleep(inputDelay);
      keybd_event(code, 0x00, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      keybd_event(160, 0x00, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }else{
      keybd_event(code, 0x00, KEYEVENTF_EXTENDEDKEY | 0, 0);
      keybd_event(code, 0x00, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }
  }

  /**
   * Simulates keypress to alt tab into the next window.
   * int count: how many windows to tab through.
   */
  void altTab(int count){
    keybd_event(18, 0x00, KEYEVENTF_EXTENDEDKEY | 0, 0);
    for(int i = 0; i < count; i++){
      keybd_event(9, 0x00, KEYEVENTF_EXTENDEDKEY | 0, 0);
      Sleep(inputDelay);
      keybd_event(9, 0x00, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
      Sleep(inputDelay);
    }
    keybd_event(18, 0x00, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
  }

  /**
   * Gets the window title of the current active window.
   * string return value: the name of the forground window. Will return a empty string if failed.
   */
  string GetForegroundWindowName(){
    char wnd_title[128];
    HWND window = GetForegroundWindow();
    int length = GetWindowText(window, wnd_title, sizeof(wnd_title));
    if(length == 0) return "";
    return wnd_title;
  }

  /**
   * Will attempt to alt tab to the window with the title that contains the given string.
   * string windowTitle: the window you will search for.
   * bool return value: will return false if failed. otherwise will return true.
   */
  bool ActivateWindow(string windowTitle){
    string firstWindow = GetForegroundWindowName();
    string currentWindow = firstWindow.c_str();
    int tabCount = 1;
    while(currentWindow.find(windowTitle) == std::string::npos){
      altTab(tabCount);
      tabCount++;
      Sleep(inputDelay);
      currentWindow = GetForegroundWindowName();
      if(firstWindow == currentWindow) return false;
    }
    return true;
  }
};

/**
 * A data structure to take care of the weekly schedule.
 */
class Schedule{
  bool schedule_table[7][24 * 4]; // if this is set to true it means that the WordHandler will send messages to the window.

public:
  /**
   * Constructor
   */
  Schedule(){
    for(int i = 0; i < 7; i++){
      for(int j = 0; j < 24 * 4; j++){
        schedule_table[i][j] = false; // default is false. meaning no sending messages.
      }
    }
  }

  /**
   * Sets schedule.
   * int week: 0 = Mon, 1 = Tue, ... , 6 = Sun
   * int time: 0:00 - 0:14 = 0, 0:15 - 0:29 = 1, ... , 23:45 - 23:59 = 23 * 4
   * bool value: the value to set the schedule.
   */
  void setSchedule(int week, int time_frame, bool value){
    schedule_table[week][time_frame] = value;
  }

  /**
   * Gets schedule.
   * int week: 0 = Sun, 1 = Mon, ... , 6 = Sat
   * int time: 0:00 - 0:14 = 0, 0:15 - 0:29 = 1, ... , 23:45 - 23:59 = 23 * 4
   * bool return value: the value of the schedule.
   */
  bool getSchedule(int week, int time_frame){
    return schedule_table[week][time_frame];
  }

};

/*
 * Will update when a tm is sent to it then if it is an appropriate time to send
 * a message then it will queue a message using a MessageSender.
 */
class WordHandler{
  string message; // the message to be sent.
  Schedule *m_schedule; // message sending schedule.
  MessageSender *messanger; // messanger for the window.
  int interval_min; // minimum message interval. in milliseconds. 300000 for 5 mins.
  int interval_max; // maximum message interval. in milliseconds. 300000 for 5 mins.
  int counter; // used to count how much time is left for the next message.
  int next_counter; // the randomly generated interval.
  tm previous; // used to store the tm when the previous call happened.

public:
  /**
   * Constructor
   */
  WordHandler(string s, Schedule *t_schedule, MessageSender *ms_tmp, int t_interval1, int t_interval2){
    message = s;
    m_schedule = t_schedule;
    messanger = ms_tmp;
    interval_min = t_interval1;
    interval_max = t_interval2;
    counter = 0;
    SetNextCounter();
  }

  /**
   * Takes tm of the current time. Then queues a message to the messageSender() if current time in schedule is true.
   * tm *time_tm: the tm for the current time. Can be taken from Timer().
   */
  void update(tm *time_tm){
    if((*m_schedule).getSchedule(time_tm->tm_wday, 4 * time_tm->tm_hour + time_tm->tm_min / 15)){
      if(CheckInterval(time_tm)){
        (*messanger).queueMessage(message);
      }
    }

    previous = *time_tm;
  }

private:
  bool CheckInterval(tm *time_tm){
    int d_day = ((time_tm->tm_wday - (&previous)->tm_wday) % 7 + 7) % 7;
    int d_hour = ((time_tm->tm_hour - (&previous)->tm_hour) % 24 + 24) % 24;
    int d_min = ((time_tm->tm_min - (&previous)->tm_min) % 60 + 60) % 60;
    int d_sec = ((time_tm->tm_sec - (&previous)->tm_sec) % 60 + 60 % 60);

    counter += d_day * 86400000 + d_hour * 3600000 + d_min * 60000 + d_sec * 1000;
    cout << "counter: " << counter << endl;

    if(counter > next_counter){
      counter = 0;
      SetNextCounter();
      return true;
    }

    return false;
  }

  void SetNextCounter(){
    next_counter = interval_min + rand() % (interval_max - interval_min);
  }
};

/**
 * Acts as the central control for all update operations.
 */
class Timer{
  time_t now_c;
  int wait_time; // the time to wait between updates. in milliseconds. recommended to set it above 60000 (60 sec).

public:
  /**
   * Constructor
   */
  Timer(int wait){
    update();
    wait_time = wait;
  }

  /**
   * Get the tm for the current time.
   * tm return value: the tm of the current time.
   */
  tm *getTM(){
    update();
    return localtime(&now_c);
  }

  /**
   * Waits until the next update cycle.
   */
  void waitNext(){
    Sleep(wait_time);
  }
private:
  /**
   * Updates the time with the current time.
   */
  void update(){
    now_c = chrono::system_clock::to_time_t(chrono::system_clock::now());
  }
};

/*
 * Will open settings.txt to interpret it and set up all MessageSender, WordHandler
 * After setting them up, will create a Timer and begin the operation.
 */
class initializer{

};

int main(int argc, char *argv[]){
  debug = true;
  Sleep(1000);
  MessageSender one = MessageSender(300, 16, "Discord");
  Timer t_new = Timer(2000);
  Schedule tmp = Schedule();
  tmp.setSchedule(4, 42, true);

  WordHandler hoge = WordHandler("bobo\n", &tmp, &one, 5000, 10000);

  hoge.update(t_new.getTM());
  for(int i = 0; i < 5; i++){
    t_new.waitNext();
    hoge.update(t_new.getTM());
    one.sendToWindow();
  }
  /*

  one.queueMessage("GAGO\n");
  one.queueMessage("$paycheck\n");
  one.queueMessage("penis\n");
  bool status = one.sendToWindow();
  cout << status << endl;
  */
  return 0;
}
