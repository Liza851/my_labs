#include "PersonRequest.h"

#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Net/HTTPServerRequest.h>

#include <Poco/Data/Statement.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

using namespace Poco::Data::Keywords;

std::string gulp(std::istream &in)
{
	std::string ret;
	char buffer[4096];
	while (in.read(buffer, sizeof(buffer)))
		ret.append(buffer, sizeof(buffer));
	ret.append(buffer, in.gcount());
	return ret;
}

namespace handlers
{

struct Person
{
    int login;
    std::string first_name;
    std::string last_name;
    int age;
};

PersonRequest::PersonRequest(Poco::Data::SessionPool& pool) : _ses{pool.get()}, _person(pool.get())
{

}
void PersonRequest::handleRequest(
	Poco::Net::HTTPServerRequest& request,
	Poco::Net::HTTPServerResponse& response)
{
	response.setStatus(Poco::Net::HTTPServerResponse::HTTP_OK);
	response.setContentType("application/json");

	std::ostream& out = response.send();

    if(request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
    {
        Poco::Net::HTMLForm form(request);
        
        if(form.has("login"))
        {
            try
            {
                std::string login = form.get("login");
            
                _person.GetLogin(login);

                Poco::JSON::Object jobj = _person.toJson();
                Poco::JSON::Stringifier::stringify(jobj, out);
            }
            catch(...)
            {
                out << "ERR";
            }
        }
        else if (form.has("first_name") && form.has("last_name"))
        {
            try
            {
                std::string first_name = form.get("first_name");
                std::string last_name = form.get("last_name");

                Poco::JSON::Array jarr = _person.LookingForByMask(first_name, last_name);
                Poco::JSON::Stringifier::stringify(jarr, out);
            }
            catch(...)
            {
                out << "ERR";
            }
        }
    }
    else if(request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
    {
        try
        {			
            std::string st = gulp(request.stream());

            _person.InsertQueue(st);

            out << "OK";
        }
        catch(...)
        {
            out << "ERR";
        }   
    }
}

}
