#include "Model.hpp"

void Model::addLogger(const ConsoleLogger & newLogger)
{
    this->consoleLogger = newLogger;
}

ConsoleLogger & Model::getLogger()
{
    return consoleLogger;
}

void Model::addEventHandler(CRString key, const EventHandler & handler) 
{
    this->callbacks[key] = handler;
}
