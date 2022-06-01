#pragma once

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Data/SessionPool.h>

namespace handlers
{

class Factory: public Poco::Net::HTTPRequestHandlerFactory
{
public:
	Factory(Poco::Data::SessionPool& pool);
	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);

private:
	Poco::Data::SessionPool& _pool;
};

} // namespace handlers
