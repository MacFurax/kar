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
      mBusSendMessage = callback;
    }
    void sendMessage(Message message) { mBusSendMessage(message); }
    void receiveMessage(Message message) 
    {
      std::cout << "Service[" << mName << "]::receiveMessage : get a new message from ["<< message.origine() <<"] via bus\n";
    }

    std::string name() { return mName; }

  protected:
    std::string mName;
    std::function<void(Message message)> mBusSendMessage;
  };



  class Bus 
  {
  public:
    Bus() {}

    void registerService( Service& service)
    {
      service.registerSendMessageMethod( std::bind(&Bus::queueMessageForSending, this, _1) );
      mServicesDictionaire[service.name()] = std::bind(&Service::receiveMessage, &service, _1);
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
    std::map<std::string, std::function<void(Message Message)>> mServicesDictionaire;

  };


  


}// kar namespace

