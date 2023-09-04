
#pragma once

#include <cstdint>
#include <string>
#include "acfw/execute/Types.h"
#include "acfw/cm/Cm.h"
#include "acfw/cm/EventConsumer.h"
#include "acfw/cm/EventProvider.h"
#include "acfw/cm/NotificationSender.h"
#include "acfw/cm/NotificationListener.h"
#include "acfw/cm/MethodCaller.h"
#include "acfw/cm/MethodCallee.h"
#include "acfw/cm/FieldConsumer.h"
#include "acfw/cm/FieldProvider.h"
#include "acfw/cm/ObjectConsumer.h"
#include "acfw/cm/ObjectProvider.h"
#include "acfw/cm/detail/CmUtils.h"



namespace sample {

    

    
    


class SampleEvent : public acfw::cm::Event {
    public:
    bool f_isAllocated = false;
        std::uint64_t f_ghmSessionId = 0;
                        
    char datetime[256];
                std::uint64_t seqNum;

    static SampleEvent* allocate();
    static void free(const SampleEvent* obj);
    void release() const noexcept override;


    private:
    static acfw::cm::BufSize serializer(acfw::cm::MethodId methodId, bool server, const acfw::cm::ServiceData* obj, acfw::cm::BufPtr buf, acfw::cm::BufSize bufSize);
    static acfw::cm::ServiceData* deserializer(acfw::cm::MethodId methodId, bool server, acfw::cm::BufPtr buf, acfw::cm::BufSize bufSize);
    static const acfw::cm::DdsMessage* ddsSerializer(acfw::cm::MethodId methodId, bool server, const acfw::cm::ServiceData* obj);
    static acfw::cm::ServiceData* ddsDeserializer(acfw::cm::MethodId methodId, bool server, void* sample);
    friend class SampleEventConsumer;
    friend class SampleEventProvider;
};


class SampleEventConsumer : public acfw::cm::Service {
    public:
        virtual ~SampleEventConsumer() noexcept;

        acfw::cm::Result create(acfw::execute::RunnableId runnableId, const char* serviceSpec) noexcept;
acfw::cm::Result destroy() noexcept;
acfw::execute::RunnableId getRunnableId() const noexcept override;
const char* getTypeName() const noexcept override;
const char* getInstanceId() const noexcept override;
acfw::cm::Result start() noexcept override;
acfw::cm::Result stop() noexcept override;
bool hasPeer() const noexcept override;
acfw::cm::Service::State getState() const noexcept override;
acfw::cm::Result setStateHandler(acfw::cm::Service::StateHandler handler, void* userData) noexcept override;
acfw::cm::Result setErrorHandler(acfw::cm::Service::ErrorHandler handler, void* userData) noexcept override;


        using EventHandler = void(*)(const SampleEvent*, void*);
        acfw::cm::Result setEventHandler(EventHandler handler, void* userData) noexcept;

    private:
        acfw::cm::Result setSerializers(acfw::cm::Serializer serializer_, acfw::cm::Deserializer deserializer_) noexcept override;
acfw::cm::Result setDdsSerializers(acfw::cm::DdsSerializer serializer_,
    acfw::cm::DdsDeserializer deserializer_,
    const acfw::cm::Service::DdsDescriptor* descriptors) noexcept override;
static void defaultStateHandler(acfw::cm::Service::State state, void* userData);


        static void defaultEventHandler(const SampleEvent* event, void* userData);

        acfw::cm::EventConsumer* m_impl = nullptr;
};

class SampleEventProvider : public acfw::cm::Service {
    public:
        virtual ~SampleEventProvider() noexcept;

        acfw::cm::Result create(acfw::execute::RunnableId runnableId, const char* serviceSpec) noexcept;
acfw::cm::Result destroy() noexcept;
acfw::execute::RunnableId getRunnableId() const noexcept override;
const char* getTypeName() const noexcept override;
const char* getInstanceId() const noexcept override;
acfw::cm::Result start() noexcept override;
acfw::cm::Result stop() noexcept override;
bool hasPeer() const noexcept override;
acfw::cm::Service::State getState() const noexcept override;
acfw::cm::Result setStateHandler(acfw::cm::Service::StateHandler handler, void* userData) noexcept override;
acfw::cm::Result setErrorHandler(acfw::cm::Service::ErrorHandler handler, void* userData) noexcept override;


        acfw::cm::Result sendEvent(SampleEvent* event) noexcept;

    private:
        acfw::cm::Result setSerializers(acfw::cm::Serializer serializer_, acfw::cm::Deserializer deserializer_) noexcept override;
acfw::cm::Result setDdsSerializers(acfw::cm::DdsSerializer serializer_,
    acfw::cm::DdsDeserializer deserializer_,
    const acfw::cm::Service::DdsDescriptor* descriptors) noexcept override;
static void defaultStateHandler(acfw::cm::Service::State state, void* userData);



        acfw::cm::EventProvider* m_impl = nullptr;
};



}
