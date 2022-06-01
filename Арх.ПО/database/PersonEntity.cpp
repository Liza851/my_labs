#include "PersonEntity.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include "../config/config.h"

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

#include <memory>
#include <amqpcpp.h>

#include <iostream>

using namespace Poco::Data::Keywords;

namespace database
{   
    Person::Person(Poco::Data::Session ses) : _ses(ses)
    {
    }
    
    Person::~Person()
    {
    }

    int Person::GetShards() 
    {
		return 2;
	}

    int Person::CalcShardNum(std::string target)
    {
        return std::hash<std::string>{}(target) % GetShards();
    }

    std::string Person::GetShardHint(size_t shard_number)
    {
        std::string result = "-- sharding:";
        result += std::to_string(shard_number);

        return result;
    }

    Poco::JSON::Object Person::toJson()
    {
        Poco::JSON::Object jobj;
        jobj.set("login", _login);
        jobj.set("first_name", _first_name);
        jobj.set("last_name", _last_name);
        jobj.set("age", _age);

        return jobj;

        //Poco::JSON::Stringifier::stringify(obj, out);
    }

    void Person::fromJSON(std::string str) {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr pObject = result.extract<Poco::JSON::Object::Ptr>();

        _login = pObject->getValue<std::string>("login");
        _first_name = pObject->getValue<std::string>("first_name");
        _last_name = pObject->getValue<std::string>("last_name");
        _age = pObject->getValue<long>("age");
    }

    bool Person::GetLoginCache(std::string login)
    {
        try
        {
            auto stream = rediscpp::make_stream("localhost", "6379");
            auto response = rediscpp::execute(*stream, "get", login);
            
            std::cout << "Get key '" << login << "': " << response.as<std::string>() << std::endl;

            fromJSON(response.as<std::string>());
            
            return true;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return false;
    }

    void Person::GetLoginDB(std::string login)
    {
        Poco::Data::Statement select(_ses);

        std::string sharding_hint = GetShardHint(CalcShardNum(login));

        select << "SELECT * FROM Person WHERE login=?" + sharding_hint,
            into(_login),
            into(_first_name),
            into(_last_name),
            into(_age),
            use(login),
            now;
        
        select.execute();
        
        Poco::Data::RecordSet rs(select);

        bool first_row = rs.moveFirst();

        if(!first_row)
            throw Poco::Data::MySQL::StatementException("row is empty");

        SetLoginCache();
    }

    void Person::SetLoginCache()
    {
        auto stream = rediscpp::make_stream("localhost", "6379");
        std::string key = _login;

        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(Person::toJson(), ss);

        std::string message = ss.str();

        auto response = rediscpp::execute(*stream, "set",
                                          key,  message, "ex", "1000");

        std::cout << "Cached:" << std::endl;
        std::cout << message << std::endl;
    }

    void Person::SaveCache()
    {
        std::string key = _login;

        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(Person::toJson(), ss);

        std::string message = ss.str();

        std::cout << message << std::endl;
    }
    
    void Person::GetLogin(std::string login)
    {
        if(!GetLoginCache(login))
            GetLoginDB(login);   
    }

    Poco::JSON::Array Person::LookingForByMask(std::string first_name, std::string last_name)
    {
        Poco::Data::Statement select(_ses);

        Poco::JSON::Array jarr;
        
        first_name += "%";
        last_name += "%";

        for (int shard = 0; shard < 2; shard++)
        {
            std::string sharding_hint = GetShardHint(shard);

            select << "SELECT * FROM Person where first_name like ? and last_name like ?" + sharding_hint,
            into(_login),
            into(_first_name),
            into(_last_name),
            into(_age),
            use(first_name),
            use(last_name),
            range(0, 1);

            while (!select.done())
            {
                select.execute();

                Poco::Data::RecordSet rs(select);

                bool first_row = rs.moveFirst();

                if(!first_row)
                    throw Poco::Data::MySQL::StatementException("row is empty");

                jarr.add(Person::toJson());
            }
        }
        return jarr;
    }

    void Person::InsertJson(std::string jstr)
    {
        std::cout << jstr << std::endl;

        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(jstr);

        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        _login = object->getValue<std::string>("login");
        _first_name = object->getValue<std::string>("first_name");
        _last_name = object->getValue<std::string>("last_name");
        _age = object->getValue<int>("age");
        
        Poco::Data::Statement insert(_ses);

        std::string sharding_hint = GetShardHint(CalcShardNum(_login));
        
        insert << "INSERT INTO Person (login, first_name, last_name, age) VALUES (?, ?, ?, ?)" + sharding_hint,
            use(_login),
            use(_first_name),
            use(_last_name),
            use(_age);
        
        insert.execute();

        SetLoginCache();
    }
    void Person::InsertQueue(std::string jstr)
    {
        static SimplePocoHandler handler(Config::get().amqp_host(), Config::get().amqp_port());

        static AMQP::Connection connection(&handler, AMQP::Login(
            Config::get().amqp_login(), Config::get().amqp_password()), Config::get().url()
        );
        static AMQP::Channel channel(&connection);

        channel.onReady(
            [&]()
            {
                std::cout << "ready" << std::endl;
                    channel.publish("", Config::get().topic(), jstr.c_str());
                    std::cout << " [x] Sent " << std::endl;
                handler.quit(); 
            });

        handler.loop();
    }
    
} // namespace database
