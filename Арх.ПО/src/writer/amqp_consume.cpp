#include <iostream>
#include <csignal>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/SessionPool.h>

#include <Poco/Util/Option.h>
#include <Poco/Util/OptionCallback.h>

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/MySQL/MySQLException.h>

#include <boost/program_options.hpp>

#include "../../database/PersonEntity.h"

#include "SimplePocoHandler.h"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    std::string input;
    std::string output;
    // Configure options here
    po::options_description desc ("Allowed options");
    desc.add_options()
    ("help,h", "This screen")
    ("url,", po::value<std::string>()->required(), "set amqp url")
    ("amqp_host,", po::value<std::string>()->required(), "set amqp host")
    ("amqp_port,", po::value<int>()->required(), "set amqp port")
    ("amqp_login,", po::value<std::string>()->required(), "set amqp login")
    ("amqp_password,", po::value<std::string>()->required(), "set amqp password")
    ("host,", po::value<std::string>()->required(), "host name")
    ("port,", po::value<std::string>()->required(), "database port")
    ("login,", po::value<std::string>()->required(), "database login")
    ("password,", po::value<std::string>()->required(), "database password")
    ("database,", po::value<std::string>()->required(), "database name")
    ("topic,", po::value<std::string>()->required(), "topic name");

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help"))
            std::cout << desc << '\n';

    Poco::Data::MySQL::Connector::registerConnector();
		
    std::stringstream ss;
    ss << "host=" << vm["host"].as<std::string>() << ";"
        << "port=" << vm["port"].as<std::string>() << ";"
        << "db=" << vm["database"].as<std::string>() << ";"
        << "user=" << vm["login"].as<std::string>() << ";"
        << "password=" << vm["password"].as<std::string>() << ";";

    std::string conn = ss.str() + "compress=true;auto-reconnect=true";

    std::cout << conn << std::endl;

    Poco::Data::SessionPool pool("MySQL", conn);

    database::Person _person(pool.get());

    // _person.GetLogin("boba");
    
    // std::stringstream ss_;
    // Poco::JSON::Object jobj = _person.toJson();
    // Poco::JSON::Stringifier::stringify(jobj, ss_);


    // std::cout << "It's me!" << std::endl;
    // std::cout << ss_.str() << std::endl;
    
    SimplePocoHandler handler(vm["amqp_host"].as<std::string>(), vm["amqp_port"].as<int>());

    AMQP::Connection connection(&handler, AMQP::Login(
        vm["amqp_login"].as<std::string>(), vm["amqp_password"].as<std::string>()), 
        vm["url"].as<std::string>()
    );

    AMQP::Channel channel(&connection);
    channel.declareQueue(vm["topic"].as<std::string>());
    channel.consume(vm["topic"].as<std::string>(), AMQP::noack).onReceived(
        [&](const AMQP::Message &message, [[maybe_unused]] uint64_t deliveryTag, [[maybe_unused]] bool redelivered)
        {
            std::string msg;
            for(size_t i=0;i<message.bodySize();++i) 
                msg+=message.body()[i];
            
            try
            {			
                _person.InsertJson(msg);
                std::cout << " [*] Sended\n";
            }
            catch(...)
            {
                std::cout << " [*] ERRR\n";
            }   
                
        });

    std::cout << " [*] Waiting for messages. To exit press CTRL-C\n";
    while (true)
        handler.loop();
    return 0;

}