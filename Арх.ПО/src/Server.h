#pragma once

#include <Poco/Util/ServerApplication.h>
#include <Poco/Util/OptionSet.h>

#include "../config/config.h"

class Server : public Poco::Util::ServerApplication
{
public:
    Server();

    ~Server();

protected:
    void initialize(Application &self);

    void uninitialize();

    void defineOptions(Poco::Util::OptionSet &options);

    void handleHost([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleLogin([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handlePassword([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleDatabase([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handlePort([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);

    void handleAmqpHost([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleAmqpPort([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleAmqpLogin([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleAmqpPassword([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleAmqpTopic([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);
    void handleAmqpUrl([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value);

    int main([[maybe_unused]] const std::vector<std::string> &args);

private:
    bool _helpRequested;
    std::string _host;
    std::string _port;
    std::string _login;
    std::string _password;
    std::string _database;
};
