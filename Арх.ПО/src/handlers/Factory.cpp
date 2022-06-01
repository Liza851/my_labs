#include "Factory.h"

#include "PersonRequest.h"

#include <Poco/Net/HTTPServerRequest.h>

namespace handlers
{

static bool startsWith(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

Factory::Factory(Poco::Data::SessionPool& pool) : _pool {pool}
{

}
Poco::Net::HTTPRequestHandler* Factory::createRequestHandler(
	const Poco::Net::HTTPServerRequest& request)
{

	std::string math = "/person";

	if (startsWith(request.getURI(), "/person"))
		return new PersonRequest(_pool);

	return nullptr;
}

} // namespace handlers
