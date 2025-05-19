#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <map>
#include "Engine/Core/HashedCaseInsensitiveString.hpp"

class EventRecipient
{
public:
	virtual ~EventRecipient();
	virtual void Execute(const EventArgs& arg) = 0;
};

template <typename T>
class EventReceiver : public EventRecipient
{
public:
	typedef bool (T::* MemberEventCallback)(const EventArgs&);

	EventReceiver(T* obj, MemberEventCallback func)
		: m_obj(obj), m_func(func) {}

	virtual void Execute(const EventArgs& arg) override
	{
		if (m_obj && m_func)
			(m_obj->*m_func)(arg);
	}
	
	bool Match(const EventReceiver* rhs) 
	{
		return m_obj == rhs->m_obj && m_func == rhs->m_func;
	}

private:
	T* m_obj;
	MemberEventCallback m_func;
};

struct EventSystemConfig
{

};

class EventSystem
{
public:

	EventSystem(EventSystemConfig const& config);
	~EventSystem();
	void StartUp();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr);
	
	template<typename T>
	void SubscribeEventCallbackFunction(HashedCaseInsensitiveString const& key, T* object, bool (T::* callback)(const EventArgs&));
	template<typename T>
	void SubscribeEventCallbackFunction(std::string const& eventName, T* object, bool (T::* callback)(const EventArgs&));
	template<typename T>
	void UnsubscribeEventCallbackFunction(std::string const& eventName, T* object, bool (T::* callback)(const EventArgs&));

	void UnSubscribeEventRecipient(EventRecipient* event);

	void FireEvent(std::string const& eventName, EventArgs& args);													// For have parameters functions
	void FireEvent(std::string const& eventName);																	// For no parameter functions


	std::vector<std::string> GetAllDevConsoleRegisteredCommands();

protected:
	EventSystemConfig									m_config;
	std::recursive_mutex								m_subscriptionListMutex;
	std::map<std::string, SubscriptionList>				m_subscriptionListByEventName;
	std::map<HashedCaseInsensitiveString,std::vector<EventRecipient*>> m_memberFunctionSubscriptionList;
private:
};

template<typename T>
void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, T* object, bool (T::* callback)(const EventArgs&))
{
	HashedCaseInsensitiveString key(eventName);
	EventRecipient* recipient = new EventReceiver<T>(object, callback);
	m_memberFunctionSubscriptionList[key].push_back(recipient);
}

template<typename T>
void EventSystem::SubscribeEventCallbackFunction(HashedCaseInsensitiveString const& key, T* object, bool (T::* callback)(const EventArgs&))
{
	EventRecipient* recipient = new EventReceiver<T>(object, callback);
	m_memberFunctionSubscriptionList[key].push_back(recipient);
}

template<typename T>
void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, T* object, bool (T::* callback)(const EventArgs&))
{
	HashedCaseInsensitiveString key(eventName);
	auto it = m_memberFunctionSubscriptionList.find(key);
	if (it != m_memberFunctionSubscriptionList.end())
	{
		auto& vec = it->second;
		for (auto iter = vec.begin(); iter != vec.end(); )
		{
			EventReceiver<T>* receiver = dynamic_cast<EventReceiver<T>*>(*iter);
			if (receiver && receiver->Matches(object, callback))
			{
				iter = vec.erase(iter);
				delete receiver;
			}
			else
			{
				++iter;
			}
		}
		if (vec.empty())
		{
			m_memberFunctionSubscriptionList.erase(it);
		}
	}
}

///---------------------------------------------------------------------------------------------------------------------------------
/// Standalone global namespace helper functions, find the event system and call the function inside it
/// 
void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr);
void FireEvent(std::string const& eventName, EventArgs& args);													
void FireEvent(std::string const& eventName);