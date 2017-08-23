#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <queue>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <map>
#include <functional>

#include <gsl\gsl>

#include <g3log\g3log.hpp>
#include <g3log/logworker.hpp>

namespace kar {

	// needed to use std::bind
  using namespace std::placeholders;

  class Base
  {
	public:
		Base()
		{
			
			if (firstInit)
			{
				firstInit = false;
				//std::cout << "Base first init\n";

				// initialize once member variables/objects

				static auto worker = g3::LogWorker::createLogWorker();
				static auto handle = worker->addDefaultLogger("kar", "./");
				g3::initializeLogging(worker.get());

				LOG(INFO) << "Logging initialized";

			}
			else 
			{
				// every time initialization
				//std::cout << "Base already initialized\n";
			}
		}

	protected:
		static bool firstInit;

  };

	bool Base::firstInit = true;



  class Message : public Base
  {
  public:
    Message() {}
    Message(std::string target, std::string origine) : mTarget(target), mOrigine(origine) {}
    Message(std::string target, std::string origine, std::vector<char>& data) : mTarget(target), mOrigine(origine), mData(data) {}

    std::string target() { return mTarget; }
    std::string origine() { return mOrigine; }
    std::vector<char> data() { return mData; }

  protected:
    std::string mTarget  = "";
    std::string mOrigine = "";
    std::vector<char> mData;
  };


  class Service : public Base
  {
  public:
    Service(std::string name) : mName(name) {}

    void registerSendMessageMethod(std::function<void(Message message)> callback) 
    { 
      mNodeSendMessage = callback;
    }
    void sendMessage(Message message) { mNodeSendMessage(message); }
    void receiveMessage(Message message) 
    {
      std::cout << "Service[" << mName << "]::receiveMessage : get a new message from ["<< message.origine() <<"] via node\n";
			LOG(INFO) << "Service[" << mName << "]::receiveMessage : get a new message from [" << message.origine() << "] via node\n";
    }

    std::string name() { return mName; }

  protected:
    std::string mName;
    std::function<void(Message message)> mNodeSendMessage;
  };


  class Node : public Base
  {
  public:
	Node(std::string name) : mName{name} {}

    bool registerService( Service& service)
    {
			bool registered = false;

      if( mServicesDictionaire[service.name()] )
			{
				std::cout << "WARNING : Service [" << service.name() << "] already registered\n";
				LOG(WARNING) << "Service [" << service.name() << "] already registered\n";
      }
      else 
      {

				// using std::bind
				//service.registerSendMessageMethod( std::bind(&Bus::queueMessageForSending, this, _1) );
				//mServicesDictionaire[service.name()] = std::bind(&Service::receiveMessage, &service, _1);

				// using lambda
				service.registerSendMessageMethod([this](Message message)-> void { this->queueMessageForSending(message); });
				mServicesDictionaire[service.name()] = [&service](Message message) -> void { service.receiveMessage(message); };
				registered = true;

      }
			return registered;
    }

		// this method will be passed to the services to allow them to send message to the bus trough the Node
    void queueMessageForSending(Message message) 
    {
      std::cout << "Bus::queueMessageForSending : Queue message from [" << message.origine() <<"] to ["<< message.target() <<"] for sending \n";
    }

		// this used the service registered receiveMessage to send them message targeted to them
    void giveMessageToService( std::string serviceName, Message message) 
    {
      std::function<void(Message Message)> serviceReceiveMessage = mServicesDictionaire[serviceName];
      if (serviceReceiveMessage)
      {
        serviceReceiveMessage(message);
      }
    }

  protected:
		std::string mName;
    std::map<std::string, std::function<void(Message Message)>> mServicesDictionaire;
  };

  class TransportBase : public Base
  {

  };

  class MessageCoderDecoderBase : public Base
  {

  };

  class Bus : public Base
  {

  };


  class System : public Base
  {

  };
  
}// kar namespace

