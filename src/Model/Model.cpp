#include "Model.hpp"

void Model::addAuth(const std::string & key, const std::string & token)
{
    TDAInterface.addAuth(key, token);
}

void Model::addLogger(const Premia::ConsoleLogger & newLogger)
{
    this->consoleLogger = newLogger;
}

Premia::ConsoleLogger & Model::getLogger()
{
    return consoleLogger;
}

void Model::addEventHandler(const std::string & key, const Premia::EventHandler & handler) 
{
    this->callbacks[key] = handler;
}

tda::TDAmeritrade & Model::getTDAInterface()
{
    return TDAInterface;
}
