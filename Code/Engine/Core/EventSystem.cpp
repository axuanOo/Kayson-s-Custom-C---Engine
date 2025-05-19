#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/NamedProperties.hpp"

EventSystem* g_theEventSystem = nullptr;

EventSystem::EventSystem(EventSystemConfig const& config)
	:m_config(config)
{

}

EventRecipient::~EventRecipient()
{
	g_theEventSystem->UnSubscribeEventRecipient(this);
}

EventSystem::~EventSystem()
{
	for (auto& pair : m_memberFunctionSubscriptionList)
	{
		for (EventRecipient* recipient : pair.second)
		{
			delete recipient;
		}
	}
}

void EventSystem::StartUp()
{

}

void EventSystem::Shutdown()
{
}

void EventSystem::BeginFrame()
{
}

void EventSystem::EndFrame()
{
}

void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr)
{
	m_subscriptionListMutex.lock();

	auto iter = m_subscriptionListByEventName.find(eventName);

	if (iter != m_subscriptionListByEventName.end())
	{
		SubscriptionList& subscription = iter->second;

		for (int i = 0; i < subscription.size(); ++i)
		{
			if (subscription[i] == nullptr)
			{
				subscription[i] = functionPtr;
				m_subscriptionListMutex.unlock();
				return;
			}
		}
		subscription.push_back(functionPtr);
	}
	else 
	{
		m_subscriptionListByEventName[eventName].push_back(functionPtr);
	}

	m_subscriptionListMutex.unlock();
}

void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr)
{
	m_subscriptionListMutex.lock();

	auto iter = m_subscriptionListByEventName.find(eventName);

	if (iter != m_subscriptionListByEventName.end()) 
	{
		SubscriptionList& subscription = iter->second;

		for (int i = 0; i < subscription.size(); ++i)
		{
			if (functionPtr == subscription[i])
			{
				subscription[i] = nullptr;
			}
		}
	}
	m_subscriptionListMutex.unlock();
}

void EventSystem::UnSubscribeEventRecipient(EventRecipient* event)
{
	for (auto& pair : m_memberFunctionSubscriptionList) {
		auto& vec = pair.second;
		vec.erase
		(
			std::remove(vec.begin(), vec.end(), event),
			vec.end()
		);
	}
}

void EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{
	m_subscriptionListMutex.lock();

	bool hasEvent = false;

	for (auto it = m_subscriptionListByEventName.begin(); it != m_subscriptionListByEventName.end(); ++it) 
	{
		if (ToLower(it->first) == ToLower(eventName))
		{
			SubscriptionList& subscription = it->second;

			if ((subscription.size() == 0))
			{
				g_theConsole->AddLine(g_theConsole->ERROR, "NO REGISTERED SUBSCRIBERS!");
				m_subscriptionListMutex.unlock();
				return;
			}

			hasEvent = true;
			for (int i = 0; i < subscription.size(); ++i)
			{
				if (subscription[i] != nullptr)
				{
					subscription[i](args);
				}
			}
		}
	}

	HashedCaseInsensitiveString key(eventName);
	auto it = m_memberFunctionSubscriptionList.find(key);
	if (it != m_memberFunctionSubscriptionList.end())
	{
		for (EventRecipient* recipient : it->second)
		{
			if (recipient)
				hasEvent = true;
				recipient->Execute(args);
		}
	}

	if (hasEvent == false)
	{
		if (g_theConsole!=nullptr)
		{
			g_theConsole->AddLine(DevConsole::WARNING, eventName);
			g_theConsole->AddLine(g_theConsole->ERROR, "NO SUCH COMMAND IN THE COMMAND LIST!");
		}
	}
	m_subscriptionListMutex.unlock();
}

void EventSystem::FireEvent(std::string const& eventName)
{
	EventArgs nullStrings;
	FireEvent(eventName, nullStrings);
}


std::vector<std::string> EventSystem::GetAllDevConsoleRegisteredCommands()
{
	std::vector<std::string> commandList;
	commandList.reserve(m_subscriptionListByEventName.size());

	for (const auto& keyPair : m_subscriptionListByEventName)
	{
		commandList.push_back(keyPair.first);
	}

	return commandList;
}

void SubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr)
{
	g_theEventSystem->SubscribeEventCallbackFunction(eventName, functionPtr);
}

void UnsubscribeEventCallbackFunction(std::string const& eventName, EventSystemCallbackFunction functionPtr)
{
	g_theEventSystem->UnsubscribeEventCallbackFunction(eventName, functionPtr);
}

void FireEvent(std::string const& eventName, EventArgs& args)
{
	g_theEventSystem->FireEvent(eventName, args);
}

void FireEvent(std::string const& eventName)
{
	g_theEventSystem->FireEvent(eventName);
}


