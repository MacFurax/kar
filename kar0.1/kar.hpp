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



namespace kar {

  using namespace std::placeholders;

  class KarLoggerBase 
  {
    // base class for Kar logger 
  };

  class KarBase
  {
    // define logging base methods
    // ...
  };

  class Message 
  {
  public:
    Message() {}
    Message(std::string target, std::string origine) : mTarget(target), mOrigine(origine) {}
    Message(std::string target, std::string origine, std::vector<char>& data) : mTarget(target), mOrigine(origine), mData(data) {}

    std::string target() { return mTarget; }
    std::string origine() { return mOrigine; }
    std::vector<char> data() { return mData; }

  protected:
    std::string mTarget = "";
    std::string mOrigine= "";
    std::vector<char> mData;
  };


  class Service
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
    }

    std::string name() { return mName; }

  protected:
    std::string mName;
    std::function<void(Message message)> mNodeSendMessage;
  };


  class Node 
  {
  public:
	Node(std::string name) : mName{name} {}

    void registerService( Service& service)
    {
		  // using bind
		  //service.registerSendMessageMethod( std::bind(&Bus::queueMessageForSending, this, _1) );
		  //mServicesDictionaire[service.name()] = std::bind(&Service::receiveMessage, &service, _1);

		  // using lambda
		  service.registerSendMessageMethod([this](Message message)-> void { this->queueMessageForSending( message); });
      
      if( mServicesDictionaire[service.name()]){
		    mServicesDictionaire[service.name()] = [&service](Message message) -> void { service.receiveMessage(message); };
      }
      else 
      {
        // output wrning message
      }
    }

    void queueMessageForSending(Message message) 
    {
      std::cout << "Bus::queueMessageForSending : Queue message from [" << message.origine() <<"] to ["<< message.target() <<"] for sending \n";
    }

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

  class TransportBase
  {

  };

  class MessageCoderDecoderBase
  {

  };

  class Bus 
  {

  };


  class System 
  {

  };
  
}// kar namespace

