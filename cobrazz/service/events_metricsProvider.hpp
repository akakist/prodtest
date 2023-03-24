#ifndef __________metricsProvider_EventInfo__HH
#define __________metricsProvider_EventInfo__HH


inline std::set<EVENT_id> getEvents_metricsProvider()
{

	std::set<EVENT_id> out;
	out.insert(MetricsProviderEventEnum::AddProvider);
	out.insert(httpEventEnum::DoListen);
	out.insert(httpEventEnum::RequestIncoming);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(systemEventEnum::startService);

	return out;
}

inline void regEvents_metricsProvider()
{
	iUtils->registerEvent(MetricsProviderEvent::AddProvider::construct);
	iUtils->registerEvent(httpEvent::DoListen::construct);
	iUtils->registerEvent(httpEvent::RequestIncoming::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
}
#endif
