#include "Server.h"

#include "handlers/Factory.h"

#include <iostream>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerParams.h>

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/SessionPool.h>

#include <Poco/Util/Option.h>
#include <Poco/Util/OptionCallback.h>

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/MySQL/MySQLException.h>

#include "writer/SimplePocoHandler.h"
#include <memory>
#include <amqpcpp.h>

Server::Server() : _helpRequested(false)
{
}

Server::~Server()
{
}

void Server::initialize(Application &self)
{
	loadConfiguration();
	ServerApplication::initialize(self);
}

void Server::uninitialize()
{
	ServerApplication::uninitialize();
}

void Server::defineOptions(Poco::Util::OptionSet &options)
{
	ServerApplication::defineOptions(options);

	options.addOption(
		Poco::Util::Option("host", "h", "set ip address for dtabase")
			.required(false)
			.repeatable(false)
			.argument("value")
			.callback(Poco::Util::OptionCallback<Server>(this, &Server::handleHost)));
	options.addOption(
            Poco::Util::Option("port", "po", "set mysql port")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handlePort)));
        options.addOption(
            Poco::Util::Option("login", "lg", "set mysql login")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleLogin)));
        options.addOption(
            Poco::Util::Option("password", "pw", "set mysql password")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handlePassword)));
        options.addOption(
            Poco::Util::Option("database", "db", "set mysql database")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleDatabase)));
        options.addOption(
            Poco::Util::Option("amqp_host", "qh", "set ampq host")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleAmqpHost)));
        options.addOption(
            Poco::Util::Option("amqp_port", "qp", "set amqp port")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleAmqpPort)));
        options.addOption(
            Poco::Util::Option("amqp_login", "ql", "set amqp login")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleAmqpLogin)));
        options.addOption(
            Poco::Util::Option("amqp_password", "qpw", "set amqp password")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleAmqpPassword)));
        options.addOption(
            Poco::Util::Option("url", "qu", "set url")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleAmqpUrl)));
        options.addOption(
            Poco::Util::Option("topic", "qt", "topic")
                .required(false)
                .repeatable(false)
                .argument("value")
                .callback(Poco::Util::OptionCallback<Server>(this, &Server::handleAmqpTopic)));
}

void Server::handleHost([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "host:" << value << std::endl;
	_host = value;
	// Config::get().host() = value;
}
void Server::handleLogin([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "login:" << value << std::endl;
	_login = value;
	// Config::get().host() = value;
}
void Server::handlePassword([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "password:" << value << std::endl;
	_password = value;
	// Config::get().host() = value;
}
void Server::handleDatabase([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "database:" << value << std::endl;
	_database = value;
	// Config::get().host() = value;
}
void Server::handlePort([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "port:" << value << std::endl;
	_port = value;
	// Config::get().host() = value;
}

void Server::handleAmqpHost([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "amqp_host:" << value << std::endl;
	Config::get().amqp_host() = value;
}
void Server::handleAmqpLogin([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "amqp_login:" << value << std::endl;
	Config::get().amqp_login() = value;
}
void Server::handleAmqpPassword([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "amqp_password:" << value << std::endl;
	Config::get().amqp_password() = value;
}
void Server::handleAmqpTopic([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "amqp_topic:" << value << std::endl;
	Config::get().topic() = value;
}
void Server::handleAmqpUrl([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "amqp_url:" << value << std::endl;
	Config::get().url() = value;
}
void Server::handleAmqpPort([[maybe_unused]] const std::string &name, [[maybe_unused]] const std::string &value)
{
	std::cout << "amqp_port:" << value << std::endl;
	Config::get().amqp_port() = std::stoi(value);
}

int Server::main([[maybe_unused]] const std::vector<std::string> &args)
{
	if (!_helpRequested)
	{
		unsigned short port = (unsigned short)config().getInt("HTTPWebServer.port", 8080);

		Poco::Data::MySQL::Connector::registerConnector();
		//std::format()
		std::stringstream ss;
		ss << "host=" << "127.0.0.1" << ";"
			<< "port=" << "6033" << ";"
			<< "db=" << _database << ";"
			<< "user=" << _login << ";"
			<< "password=" << _password << ";";

		std::string connection = ss.str() + "compress=true;auto-reconnect=true";

		std::cout << connection << std::endl;

       	Poco::Data::SessionPool pool("MySQL", connection);

		Poco::Data::Session _ses(pool.get());
		
		Poco::Data::Statement select(_ses);

		Poco::Net::ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", port));
		Poco::Net::HTTPServer srv(new handlers::Factory(pool), svs, new Poco::Net::HTTPServerParams);

		std::cout << "Started server on port: 8080" << std::endl;
		srv.start();

		waitForTerminationRequest();
		srv.stop();
	}
	return Application::EXIT_OK;
}
