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
      }else{
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

    cout << wnd_title << endl;
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
    cout << currentWindow << endl;
    return true;
  }
};

/**
 * Keeps track of a string to be send on schedule.
 */
class Schedule{

};

class WordHandler{
  string message;

public:
  void update(){

  }
};

int main(int argc, char *argv[]){
  debug = true;
  Sleep(1000);
  MessageSender one(300, 16, "Discord");
  one.queueMessage("GAGO\n");
  one.queueMessage("$paycheck\n");
  one.queueMessage("penis\n");
  bool status = one.sendToWindow();
  cout << status << endl;
  return 0;
}
