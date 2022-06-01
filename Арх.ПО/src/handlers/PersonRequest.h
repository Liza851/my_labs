#pragma once
#include "../../database/PersonEntity.h"
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Data/SessionPool.h>
#include <Poco/Data/Session.h>
#include <iostream>

namespace handlers
{

class PersonRequest: public Poco::Net::HTTPRequestHandler
{
public:
	PersonRequest(Poco::Data::SessionPool& pool);

private:
	void handleRequest(
		Poco::Net::HTTPServerRequest& request,
		Poco::Net::HTTPServerResponse& response) override;
	Poco::Data::Session _ses;
	database::Person _person;
};

}
