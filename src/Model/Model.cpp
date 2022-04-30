#include "Model.hpp"

void Model::addLogger(const ConsoleLogger & newLogger)
{
    this->consoleLogger = newLogger;
}

ConsoleLogger & Model::getLogger()
{
    return consoleLogger;
}

void Model::addEventHandler(const std::string & key, const VoidEventHandler & handler) 
{
    this->callbacks[key] = handler;
}

tda::TDAmeritrade & Model::getTDAInterface()
{
    return TDAInterface;
}
