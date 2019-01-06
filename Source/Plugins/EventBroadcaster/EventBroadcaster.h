/*
  ==============================================================================

    EventBroadcaster.h
    Created: 22 May 2015 3:31:50pm
    Author:  Christopher Stawarz

  ==============================================================================
*/

#ifndef EVENTBROADCASTER_H_INCLUDED
#define EVENTBROADCASTER_H_INCLUDED

#include <ProcessorHeaders.h>

#ifdef ZEROMQ
    #ifdef WIN32
        #include <zmq.h>
        #include <zmq_utils.h>
    #else
        #include <zmq.h>
    #endif
#endif

#include <memory>

class EventBroadcaster : public GenericProcessor
{
public:
    EventBroadcaster();

    AudioProcessorEditor* createEditor() override;

    int getListeningPort() const;
    // returns 0 on success, else the errno value for the error that occurred.
    int setListeningPort (int port, bool forceRestart = false);

    void process (AudioSampleBuffer& continuousBuffer) override;
    void handleEvent (const EventChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;
	void handleSpike(const SpikeChannel* channelInfo, const MidiMessage& event, int samplePosition = 0) override;

    void saveCustomParametersToXml (XmlElement* parentElement) override;
    void loadCustomParametersFromXml() override;


private:
    class ZMQContext
    {
    public:
        ZMQContext();
        ~ZMQContext();
        void* createZMQSocket();
    private:
        void* context;
    };

    static void closeZMQSocket(void* socket);

    class ZMQSocketPtr : public std::unique_ptr<void, decltype(&closeZMQSocket)>
    {
    public:
        ZMQSocketPtr();
        ~ZMQSocketPtr();
    private:
        SharedResourcePointer<ZMQContext> context;
    };
    
    int unbindZMQSocket();
    int rebindZMQSocket();

	void sendEvent(const MidiMessage& event, float eventSampleRate) const;
    static String getEndpoint(int port);
    // called from getListeningPort() depending on success/failure of ZMQ operations
    void reportActualListeningPort(int port);

    ZMQSocketPtr zmqSocket;
    int listeningPort;
};


#endif  // EVENTBROADCASTER_H_INCLUDED
