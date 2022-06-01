#include "config.h"

Config::Config()
{
}

Config &Config::get()
{
    static Config _instance;
    return _instance;
}

int &Config::amqp_port()
{
    return _amqp_port;
}

std::string &Config::amqp_host()
{
    return _amqp_host;
}

std::string &Config::amqp_login()
{
    return _amqp_login;
}

std::string &Config::amqp_password()
{
    return _amqp_password;
}

std::string &Config::url()
{
    return _url;
}

std::string &Config::topic()
{
    return _topic;
}
