#include "Model.hpp"

void Model::addLogger(const Logger& newLogger)
{
    this->consoleLogger = newLogger;
}

Logger& Model::getLogger()
{
    return consoleLogger;
}

void Model::addEventHandler(CRString key, const EventHandler & handler) 
{
    this->callbacks[key] = handler;
}
