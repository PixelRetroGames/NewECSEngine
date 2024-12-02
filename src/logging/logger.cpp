#include "logger.h"

namespace LOG {

void Logger::Init() {
    ConsolePainter.Init();
    numberOfLoggers = 0;
    StartThread();
}

void Logger::AddLogger(std::string name, FILE *out, bool fileAndConsole) {
    if (loggers.count(name) == 0) {
        loggers[name] = new SpecializedLogger(name, &ConsolePainter, out, fileAndConsole, numberOfLoggers++);
    }
}

SpecializedLogger *Logger::GetLogger(std::string name) {
    if (loggers.count(name))
        return loggers[name];
    else
        return NULL;
}

void Logger::Close() {
    ended = true;
    logQueue.forceUnlock();
    thread->join();
    delete thread;
    thread = nullptr;

    for (std::unordered_map<std::string, SpecializedLogger*>::iterator i = loggers.begin(); i != loggers.end(); i++) {
        delete i->second;
    }

    loggers.clear();
    SpecializedLogger::Reset_longest_name_length();
    numberOfLoggers = 0;
}

Logger *Logger::GetInstance() {
    static Logger *instance = new Logger();
    return instance;
}

void Logger::AddToQueue(LogQuery query) {
    if (!loggers.count(query.name)) {
        return;
    }

    logQueue.push(query);
}

void Logger::PrintQueue() {
    LogQuery *query = logQueue.pop();

    while (query != nullptr) {
        GetLogger(query->name)->Log(query);
        delete query;
        query = logQueue.pop();
    }
}

void Logger::StartThread() {
    thread = new std::thread(&Logger::WriteThread, Logger::GetInstance());
}

void Logger::WriteThread() {
    while (!ended) {
        PrintQueue();
    }
}

void Logger::DisableLogging() {
    for (auto &it : loggers) {
        it.second->Disable();
    }
}

void Logger::EnableLogging() {
    for (auto &it : loggers) {
        it.second->Enable();
    }
}

///For User

void Init() {
    Logger::GetInstance()->Init();
}

void Close() {
    Logger::GetInstance()->Close();
}

void AddLogger(std::string name, std::string filename, bool fileAndConsole) {
    FILE *out = fopen(filename.c_str(), "a");
    AddLogger(name, out, fileAndConsole);
}

void AddLogger(std::string name, FILE *out, bool fileAndConsole) {
    Logger::GetInstance()->AddLogger(name, out, fileAndConsole);
}

void DisableLogging() {
    Logger::GetInstance()->DisableLogging();
}

void EnableLogging() {
    Logger::GetInstance()->EnableLogging();
}

}
