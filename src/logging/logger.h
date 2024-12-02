#pragma once

#include "thread_safe_queue.h"
#include "color.h"
#include "specialized_logger.h"
#include "log_query.h"
#include "ctimer.h"

#include <vector>
#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>

namespace LOG {

const int QUEUE_SIZE = 1024;

class Logger {
  private:
    ConsolePainter ConsolePainter;
    std::unordered_map<std::string, SpecializedLogger*> loggers;
    int numberOfLoggers = 0;
    ThreadSafeQueue<LogQuery> logQueue;
    std::thread *thread;
    std::atomic<bool> ended = false;
    long long printingIntervalMs = 0;

  private:
    void WriteThread();

  public:
    static Logger *GetInstance();
    void Init();
    void AddLogger(std::string name, FILE *out = NULL, bool fileAndConsole = false);
    SpecializedLogger *GetLogger(std::string name);
    void Close();
    void StartThread();
    void DeleteThread();
    void SetPrintingIntervalMs(long long ms);
    void AddToQueue(LogQuery query);
    void PrintQueue();
    void DisableLogging();
    void EnableLogging();

  protected:
    Logger() {}
};

///For User
void Init();
void Close();
void AddLogger(std::string name, std::string filename, bool fileAndConsole = false);
void AddLogger(std::string name, FILE *out = nullptr, bool fileAndConsole = false);
void DisableLogging();
void EnableLogging();
void SetPrintingIntervalMs(long long ms);
void StartThread();
void StopThread();

};

#define LOG_INFO(name,format,...)     LOG::Logger::GetInstance()->AddToQueue(LOG::MakeLogQuery(name,LOG::LT_INFO, __FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOG_WARNING(name,format,...)  LOG::Logger::GetInstance()->AddToQueue(LOG::MakeLogQuery(name,LOG::LT_WARNING, __FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOG_ERROR(name,format,...)    LOG::Logger::GetInstance()->AddToQueue(LOG::MakeLogQuery(name,LOG::LT_ERROR, __FILE__, __LINE__, format, ##__VA_ARGS__))
#define LOG_CRITICAL(name,format,...) LOG::Logger::GetInstance()->AddToQueue(LOG::MakeLogQuery(name,LOG::LT_CRITICAL, __FILE__, __LINE__, format, ##__VA_ARGS__))
