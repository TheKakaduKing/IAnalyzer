#pragma once
#include <iostream>
#include <thread>
#include <utility>
#include <vector>
#include <memory>


struct stThread{
  int threadId;
  int windowId;
  std::atomic<bool> running;
  std::thread thread;

  ~stThread(){
    if (thread.joinable()) {
      thread.join();
    }
  }
};

struct stThreadState{
  int threadId;
  bool running;
  double runtime;
};


//*************************************************************************************************
//                      iathread class
//*************************************************************************************************
//
class iathread{
  public:
    static iathread& instance(){
      static iathread inst;
      return inst;
    }

  public:
    template<typename T, typename R, typename... Args>
    static inline void newThread(int windowId, T* obj, R (T::*func)(Args...), Args... args);
    static inline std::vector<stThreadState> getWindowThreads(int windowId);
    static inline std::vector<int> getActiveThreads();
    

  private:
    static inline void cleanThreads();

  private:
    static inline int threadId_{0};
    static inline std::vector<std::shared_ptr<stThread>> threads_;
    static inline std::queue<int> threadDestroyQueue_; 
};


//*************************************************************************************************
//                      iathread class member declaration
//*************************************************************************************************

// ***Member function***
// --Start a new thread with a given object and given member function
// --Takes variadic arguments, can call any member function via pointer in any object
//
template<typename T, typename R, typename... Args>
inline void iathread::newThread(int windowId, T* obj, R (T::*func)(Args...), Args... args){

  cleanThreads();

  threadId_++;
 
  auto newThread = std::make_shared<stThread>();
  newThread->threadId = threadId_;
  newThread->windowId = windowId;
  newThread->running = true;
  newThread->thread = std::thread([=](){
        (obj->*func)(args...);
        newThread->running = false;
  });

  threads_.push_back(std::move(newThread));
  std::cout << "thread created: " << threads_.back()->threadId << std::endl;

}


// ***Member function***
// --Clean up threads if they are not running and joinable
//
inline void iathread::cleanThreads(){
  for (std::shared_ptr<stThread>& t : threads_) {
    if (!t->running && t->thread.joinable()) {
      // The thread gets joined in the stThread destructor
      threadDestroyQueue_.push(t->threadId);
    }  
  }
  while (!threadDestroyQueue_.empty()) {
    std::erase_if(threads_, [&] (auto& t){return t->threadId == threadDestroyQueue_.front();}); 
    std::cout << "thread destroyed: " << threadDestroyQueue_.front() << std::endl;
    threadDestroyQueue_.pop();
  }

}


// ***Member function***
// --Get the internal thread ids of a given window via window id
//
inline std::vector<stThreadState> iathread::getWindowThreads(int windowId){

  cleanThreads();

  std::vector<stThreadState> windowThreads{};
  for (std::shared_ptr<stThread>& t : threads_) {
    if (t->windowId == windowId){
      windowThreads.emplace_back(t->threadId, t->running, 0);
    }
  }
  return windowThreads;
}


// ***Member function***
// --Get all active thread ids
//
inline std::vector<int> iathread::getActiveThreads(){
  std::vector<int> active{};
  for (std::shared_ptr<stThread>& t : threads_) {
    if(t->running){
      active.push_back(t->threadId);
    }
  }
  return active;
}
