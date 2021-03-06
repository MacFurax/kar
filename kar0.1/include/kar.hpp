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
				
				mLogWorker = g3::LogWorker::createLogWorker();
				mFSHandle = mLogWorker->addDefaultLogger("kar", "./");
				g3::initializeLogging(mLogWorker.get());

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
		static std::unique_ptr<g3::LogWorker> mLogWorker;
		static std::unique_ptr<g3::FileSinkHandle> mFSHandle;
  };

	// static members of class Base
	bool Base::firstInit = true;
	std::unique_ptr<g3::LogWorker> Base::mLogWorker;
	std::unique_ptr<g3::FileSinkHandle> Base::mFSHandle;




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

  class Buffer
  {
  public:
    Buffer() {}
    Buffer(std::unique_ptr<std::vector<char>> buffer, int size)
    {
      mBuffer = std::move(buffer);
      mSize = size;
    }

    const char* data() { mBuffer->data(); }
    const int size() { return mSize; }

  private:
    std::unique_ptr<std::vector<char>> mBuffer;
    int mSize = 0;
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
      //std::cout << "Service[" << mName << "]::receiveMessage : get a new message from ["<< message.origine() <<"] via node\n";
			LOG(INFO) << "Service[" << mName << "]::receiveMessage : get a new message from [" << message.origine() << "] via node";
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
				//std::cout << "WARNING : Service [" << service.name() << "] already registered\n";
				LOG(WARNING) << "Service [" << service.name() << "] already registered";
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
      //std::cout << "Bus::queueMessageForSending : Queue message from [" << message.origine() <<"] to ["<< message.target() <<"] for sending \n";
			LOG(INFO) << "Bus::queueMessageForSending : Queue message from [" << message.origine() << "] to [" << message.target() << "] for sending";
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
  public:
    TransportBase() {}
    ~TransportBase() {}

    // this will do the work to send and receive buffers from the underlying transport
    void virtual run() {}

  private:
    int sendBuffer( std::unique_ptr<Buffer> buffer) 
    {
      mToSendBuffers.push(std::move(buffer));
      return mToSendBuffers.size();
    }

    int getBuffer(std::unique_ptr<Buffer> buffer)
    {
      if (mReceivedBuffers.size() == 0) return -1;
      buffer = std::move(mReceivedBuffers.front());
      mReceivedBuffers.pop();
      return mReceivedBuffers.size();
      return 1;
    }

    std::queue<std::unique_ptr<Buffer>> mToSendBuffers;
    std::queue<std::unique_ptr<Buffer>> mReceivedBuffers;
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

