#pragma once

#include <string>

class  Config{
    private:
        Config();
        std::string _amqp_host;
        int _amqp_port;
        std::string _amqp_login;
        std::string _amqp_password;
        std::string _url;
        std::string _topic;

        std::string _cache_servers;

    public:
        static Config& get();

        int& amqp_port();
        std::string& amqp_host();
        std::string& amqp_login();
        std::string& amqp_password();
        std::string& url();
        std::string& topic();
};

