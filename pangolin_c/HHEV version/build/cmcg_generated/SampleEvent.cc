

#include "SampleEvent.h"
extern "C" {
#include "SampleEvent_cmcg.h"
}
#include "acfw/cm/ServiceFactory.h"
#include "acfw/execute/detail/TypeInfo.h"

using namespace acfw::cm;

namespace sample {

    

    
class SampleEventDdsMessage : public acfw::cm::DdsMessage {
public:
    SampleEventDdsMessage() noexcept {
        sample = sample_SampleEvent__alloc();
    }
    virtual ~SampleEventDdsMessage() noexcept {
        sample_SampleEvent_free(sample, DDS_FREE_ALL);
    }

    void release() const noexcept override {
        delete this;
    }
};


SampleEvent* SampleEvent::allocate()
{
    auto r = new SampleEvent();
    if (r != nullptr) {
        r->f_isAllocated = true;
    }
    return r;
}

void SampleEvent::free(const SampleEvent* obj)
{
    if (obj != nullptr && obj->f_isAllocated)
        delete obj;
}

void SampleEvent::release() const noexcept
{
    SampleEvent::free(this);
}


acfw::cm::BufSize SampleEvent::serializer(acfw::cm::MethodId methodId, bool server, const acfw::cm::ServiceData* obj, acfw::cm::BufPtr buf, acfw::cm::BufSize bufSize)
{
    (void)methodId;
    (void)server;
    if (obj == nullptr || buf == nullptr || bufSize == acfw::cm::BUF_SIZE_INVALID) {
        return acfw::cm::BUF_SIZE_INVALID;
    }
    auto obj_ = dynamic_cast<const SampleEvent*>(obj);
    if (obj_ == nullptr) {
        return acfw::cm::BUF_SIZE_INVALID;
    }
    auto buf_ = (BufPtr)buf;
    BufSize bufSize_ = 0;
    (void)buf_;
    (void)bufSize_;
    if (bufSize_ + sizeof(std::uint64_t) > bufSize)
        return acfw::cm::BUF_SIZE_INVALID;
    *((std::uint64_t*)buf_) = obj_->f_ghmSessionId;
    buf_ += sizeof(std::uint64_t);
    bufSize_ += sizeof(std::uint64_t);
    
    
        // datetime
    
    memcpy(buf_, obj_->datetime, sizeof(obj_->datetime));
    bufSize_ += sizeof(obj_->datetime);

        // seqNum
    

    if (bufSize_ + sizeof(std::uint64_t) > bufSize)
        return acfw::cm::BUF_SIZE_INVALID;
    *((std::uint64_t*)buf_) = obj_->seqNum;
    buf_ += sizeof(std::uint64_t);
    bufSize_ += sizeof(std::uint64_t);


    return bufSize_;
}

acfw::cm::ServiceData* SampleEvent::deserializer(acfw::cm::MethodId methodId, bool server, acfw::cm::BufPtr buf, acfw::cm::BufSize bufSize)
{
    (void)methodId;
    (void)server;
    if (buf == nullptr || bufSize == acfw::cm::BUF_SIZE_INVALID) {
        return nullptr;
    }
    BufSize objSize = 0;
    auto buf_ = (BufPtr)buf;
    (void)objSize;
    (void)buf_;
    SampleEvent* obj = SampleEvent::allocate();

    if (objSize + sizeof(std::uint64_t) > bufSize)
        goto buffer_exhausted;
    obj->f_ghmSessionId = *((std::uint64_t*)buf_);
    buf_ += sizeof(std::uint64_t);
    objSize += sizeof(std::uint64_t);
    
    

        // datetime
        memcpy(obj->datetime, buf_, sizeof(obj->datetime));
    objSize += sizeof(obj->datetime);

        // seqNum
    

    if (objSize + sizeof(std::uint64_t) > bufSize)
        goto buffer_exhausted;
    obj->seqNum = *((std::uint64_t*)buf_);
    buf_ += sizeof(std::uint64_t);
    objSize += sizeof(std::uint64_t);


    return obj;

buffer_exhausted:
    SampleEvent::free(obj);
    return nullptr;
}


const acfw::cm::DdsMessage* SampleEvent::ddsSerializer(acfw::cm::MethodId methodId, bool server, const acfw::cm::ServiceData* obj)
{
    (void)methodId;
    (void)server;
    acfw::cm::DdsMessage* msg = new SampleEventDdsMessage;
    msg->descriptorIndex = 0;
    auto obj_ = dynamic_cast<const SampleEvent*>(obj);
    (void)obj_;
    auto sample_ = (sample_SampleEvent*)msg->sample;
    (void)sample_;
    if (obj_ != nullptr) {
        sample_->f_ghmSessionId = obj_->f_ghmSessionId;
    }
    
    if (obj_ != nullptr) {
        
    // datetime
            
    memcpy(sample_->datetime, obj_->datetime, sizeof(obj_->datetime));

    // seqNum
            
    sample_->seqNum = obj_->seqNum;


    }
    return msg;
}

acfw::cm::ServiceData* SampleEvent::ddsDeserializer(acfw::cm::MethodId methodId, bool server, void* sample)
{
    (void)methodId;
    (void)server;
    auto sample_ = (sample_SampleEvent*)sample;
    (void)sample_;
    SampleEvent* obj = SampleEvent::allocate();
    obj->f_ghmSessionId = sample_->f_ghmSessionId;
    
    
    // datetime
            
    memcpy(obj->datetime, sample_->datetime, sizeof(obj->datetime));

    // seqNum
            
    obj->seqNum = sample_->seqNum;


    return obj;
}


static const dds_topic_descriptor_t* SampleEvent_dds_descriptors[] = {
    &sample_SampleEvent_desc,
};

static const char* serviceSpec_sample_SampleEvent = R"(
    {"class": "event", "type": {"class": "struct", "members": {"datetime": {"class": "array", "type": "char", "length": 256}, "seqNum": "uint64"}}}
)";




acfw::cm::Result SampleEventConsumer::create(acfw::execute::RunnableId runnableId, const char* serviceSpec) noexcept
{
    if (m_impl != nullptr) {
        // Service already created, don't create it again and return OK
        return acfw::cm::Result::OK;
    }
    auto factory = ServiceFactory::get();
    auto service = factory->createService(runnableId, serviceSpec);
    m_impl = dynamic_cast<EventConsumer*>(service);
    if (m_impl == nullptr) {
        factory->deleteService(service);
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    m_impl->setSerializers(SampleEvent::serializer, SampleEvent::deserializer);
    m_impl->setDdsSerializers(SampleEvent::ddsSerializer, SampleEvent::ddsDeserializer, SampleEvent_dds_descriptors);
    m_impl->setStateHandler(SampleEventConsumer::defaultStateHandler, (void*)this);
    m_impl->setEventHandler((acfw::cm::EventConsumer::EventHandler)SampleEventConsumer::defaultEventHandler, (void*)this);
    return acfw::cm::Result::OK;
}

acfw::cm::Result SampleEventConsumer::destroy() noexcept
{
    if (m_impl == nullptr) {
        // Service already destroyed, happily returns
        return acfw::cm::Result::OK;
    }
    stop();
    auto factory = ServiceFactory::get();
    factory->deleteService(m_impl);
    m_impl = nullptr;
    return acfw::cm::Result::OK;
}

acfw::execute::RunnableId SampleEventConsumer::getRunnableId() const noexcept
{
    if (m_impl == nullptr) {
        return -1;
    }
    return m_impl->getRunnableId();
}

const char* SampleEventConsumer::getTypeName() const noexcept
{
    if (m_impl == nullptr) {
        return nullptr;
    }
    return m_impl->getTypeName();
}

const char* SampleEventConsumer::getInstanceId() const noexcept
{
    if (m_impl == nullptr) {
        return nullptr;
    }
    return m_impl->getInstanceId();
}

acfw::cm::Result SampleEventConsumer::start() noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->start();
}

acfw::cm::Result SampleEventConsumer::stop() noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->stop();
}

bool SampleEventConsumer::hasPeer() const noexcept
{
    if (m_impl == nullptr) {
        return false;
    }
    return m_impl->hasPeer();
}

acfw::cm::Service::State SampleEventConsumer::getState() const noexcept
{
    if (m_impl == nullptr) {
        return Service::State::IDLE;
    }
    return m_impl->getState();
}

acfw::cm::Result SampleEventConsumer::setStateHandler(acfw::cm::Service::StateHandler handler, void* userData) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setStateHandler(handler, userData);
}

acfw::cm::Result SampleEventConsumer::setErrorHandler(acfw::cm::Service::ErrorHandler handler, void* userData) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setErrorHandler(handler, userData);
}

void SampleEventConsumer::defaultStateHandler(acfw::cm::Service::State state, void* userData)
{
    auto service = (SampleEventConsumer*)userData;
    if (service != nullptr && service->m_impl != nullptr) {
        dispatchToRunnable(service->m_impl->getRunnableId(),
            acfw::execute::detail::TypeInfo<acfw::cm::Service::State>::typeId(),
            (const char*)state,
            0,
            reinterpret_cast<uintptr_t>(userData),
            0,
            (acfw::execute::SessionId)0);
    }
}

acfw::cm::Result SampleEventConsumer::setSerializers(acfw::cm::Serializer serializer_, acfw::cm::Deserializer deserializer_) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setSerializers(serializer_, deserializer_);
}

acfw::cm::Result SampleEventConsumer::setDdsSerializers(acfw::cm::DdsSerializer serializer_,
    acfw::cm::DdsDeserializer deserializer_,
    const acfw::cm::Service::DdsDescriptor* descriptors) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setDdsSerializers(serializer_, deserializer_, descriptors);
}


void SampleEventConsumer::defaultEventHandler(const SampleEvent* event, void* userData)
{
    auto service = (SampleEventConsumer*)userData;
    const auto runnableId = service->m_impl->getRunnableId();
    dispatchToRunnable(runnableId,
        acfw::execute::detail::TypeInfo<SampleEvent>::typeId(),
        (const char*)event,
        sizeof(SampleEvent),
        reinterpret_cast<uintptr_t>(userData),
        0,
        event != nullptr ? event->f_ghmSessionId : (acfw::execute::SessionId)0
        );
}

SampleEventConsumer::~SampleEventConsumer() noexcept
{
    destroy();
}

acfw::cm::Result SampleEventConsumer::setEventHandler(EventHandler handler, void* userData) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setEventHandler((EventConsumer::EventHandler)handler, userData);
}




acfw::cm::Result SampleEventProvider::create(acfw::execute::RunnableId runnableId, const char* serviceSpec) noexcept
{
    if (m_impl != nullptr) {
        // Service already created, don't create it again and return OK
        return acfw::cm::Result::OK;
    }
    auto factory = ServiceFactory::get();
    auto service = factory->createService(runnableId, serviceSpec);
    m_impl = dynamic_cast<EventProvider*>(service);
    if (m_impl == nullptr) {
        factory->deleteService(service);
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    m_impl->setSerializers(SampleEvent::serializer, SampleEvent::deserializer);
    m_impl->setDdsSerializers(SampleEvent::ddsSerializer, SampleEvent::ddsDeserializer, SampleEvent_dds_descriptors);
    m_impl->setStateHandler(SampleEventProvider::defaultStateHandler, (void*)this);
    factory->publishServiceType("sample::SampleEvent", serviceSpec_sample_SampleEvent);
    return acfw::cm::Result::OK;
}

acfw::cm::Result SampleEventProvider::destroy() noexcept
{
    if (m_impl == nullptr) {
        // Service already destroyed, happily returns
        return acfw::cm::Result::OK;
    }
    stop();
    auto factory = ServiceFactory::get();
    factory->deleteService(m_impl);
    m_impl = nullptr;
    factory->unpublishServiceType("sample::SampleEvent");
    return acfw::cm::Result::OK;
}

acfw::execute::RunnableId SampleEventProvider::getRunnableId() const noexcept
{
    if (m_impl == nullptr) {
        return -1;
    }
    return m_impl->getRunnableId();
}

const char* SampleEventProvider::getTypeName() const noexcept
{
    if (m_impl == nullptr) {
        return nullptr;
    }
    return m_impl->getTypeName();
}

const char* SampleEventProvider::getInstanceId() const noexcept
{
    if (m_impl == nullptr) {
        return nullptr;
    }
    return m_impl->getInstanceId();
}

acfw::cm::Result SampleEventProvider::start() noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->start();
}

acfw::cm::Result SampleEventProvider::stop() noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->stop();
}

bool SampleEventProvider::hasPeer() const noexcept
{
    if (m_impl == nullptr) {
        return false;
    }
    return m_impl->hasPeer();
}

acfw::cm::Service::State SampleEventProvider::getState() const noexcept
{
    if (m_impl == nullptr) {
        return Service::State::IDLE;
    }
    return m_impl->getState();
}

acfw::cm::Result SampleEventProvider::setStateHandler(acfw::cm::Service::StateHandler handler, void* userData) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setStateHandler(handler, userData);
}

acfw::cm::Result SampleEventProvider::setErrorHandler(acfw::cm::Service::ErrorHandler handler, void* userData) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setErrorHandler(handler, userData);
}

void SampleEventProvider::defaultStateHandler(acfw::cm::Service::State state, void* userData)
{
    auto service = (SampleEventProvider*)userData;
    if (service != nullptr && service->m_impl != nullptr) {
        dispatchToRunnable(service->m_impl->getRunnableId(),
            acfw::execute::detail::TypeInfo<acfw::cm::Service::State>::typeId(),
            (const char*)state,
            0,
            reinterpret_cast<uintptr_t>(userData),
            0,
            (acfw::execute::SessionId)0);
    }
}

acfw::cm::Result SampleEventProvider::setSerializers(acfw::cm::Serializer serializer_, acfw::cm::Deserializer deserializer_) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setSerializers(serializer_, deserializer_);
}

acfw::cm::Result SampleEventProvider::setDdsSerializers(acfw::cm::DdsSerializer serializer_,
    acfw::cm::DdsDeserializer deserializer_,
    const acfw::cm::Service::DdsDescriptor* descriptors) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    return m_impl->setDdsSerializers(serializer_, deserializer_, descriptors);
}


SampleEventProvider::~SampleEventProvider() noexcept
{
    destroy();
}

acfw::cm::Result SampleEventProvider::sendEvent(SampleEvent* event) noexcept
{
    if (m_impl == nullptr) {
        return acfw::cm::Result::INTERNAL_ERROR;
    }
    if (event != nullptr) {
        const auto runnableId = m_impl->getRunnableId();
        event->f_ghmSessionId = getGhmSessionId(runnableId,
            acfw::execute::detail::TypeInfo<SampleEvent>::typeId());
    }
    return m_impl->sendEvent(event);
}



}
