#define WIN32_LEAN_AND_MEAN	
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#undef  ERROR

#include "Engine/Core/NetSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Game/EngineBuildPreferences.hpp"

NetSystem::NetSystem(const NetSystemConfig& config)
	:m_config(config),
	m_mode(NetSystemMode::NONE)
{
	if (config.m_modeString == "Client")
	{
		m_mode = NetSystemMode::CLIENT;
	}
	else if (config.m_modeString == "Server")
	{
		m_mode = NetSystemMode::SERVER;
	}
	else
	{
		m_mode = NetSystemMode::NONE;
	}

	IN_ADDR address;
	size_t colonPos = m_config.m_hostAddressString.find(':');
	std::string ipaddress = m_config.m_hostAddressString.substr(0, colonPos);
	std::string portString = m_config.m_hostAddressString.substr(colonPos + 1);

	int result = inet_pton(AF_INET, ipaddress.c_str(), &address);
	if (result == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		ERROR_AND_DIE(Stringf("Cannot create host address with error code: %i", errorCode));
	}
	m_hostAddress = address.S_un.S_addr;
	m_hostPort = htons(static_cast<unsigned short>(std::stoi(portString)));

	m_sendBuffer = new char[m_config.m_sendBufferSize];
	m_recvBuffer = new char[m_config.m_recvBufferSize];

}

NetSystem::~NetSystem()
{
	delete m_sendBuffer;
	delete m_recvBuffer;
}

void NetSystem::StartUp()
{
#if !defined (ENGINE_DISABLE_NETWORK)

	// Start up the windows socket
	WSADATA wsaData;
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		ERROR_AND_DIE(Stringf("Cannot create the socket for the client with error code: %i", errorCode));
	}

	if (m_mode == NetSystemMode::CLIENT)
	{
		m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		unsigned long blockingMode = 1;
		ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
		m_clientState = ClientState::DISCONNECTED;
	}
	else if (m_mode == NetSystemMode::SERVER)
	{
		m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		unsigned long blockingMode = 1;
		ioctlsocket(m_listenSocket, FIONBIO, &blockingMode);

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = (m_hostAddress);
		addr.sin_port = m_hostPort;

		int bindResult = bind(m_listenSocket, (sockaddr*)&addr, (int)sizeof(addr));
		if (bindResult == SOCKET_ERROR)
		{
			ERROR_AND_DIE("Cannot bind socket");
		}
		int result = listen(m_listenSocket, SOMAXCONN);

		if (result == SOCKET_ERROR)
		{
			ERROR_AND_DIE("Cannot listen to the client");
		}
		m_serverState = ServerState::LISTENING;

	}

#else
	return;
#endif
}

void NetSystem::ShutDown()
{
#if !defined (ENGINE_DISABLE_NETWORK)
	if (m_mode == NetSystemMode::CLIENT)
	{
		closesocket(m_clientSocket);
	}
	if (m_mode == NetSystemMode::SERVER)
	{
		closesocket(m_clientSocket);
		closesocket(m_listenSocket);
	}
	WSACleanup();
#else
	return;
#endif
}

void NetSystem::BeginFrame()
{
#if !defined (ENGINE_DISABLE_NETWORK)
	if (m_mode == NetSystemMode::CLIENT)
	{
		if (m_clientState == ClientState::DISCONNECTED)
		{
			if (m_clientSocket == INVALID_SOCKET)
			{
				m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				unsigned long blockingMode = 1;
				ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
			}

			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.S_un.S_addr = (m_hostAddress);
			addr.sin_port = (m_hostPort);
			int result = connect(m_clientSocket, (sockaddr*)(&addr), (int)sizeof(addr));

			if (result == SOCKET_ERROR)
			{
				int errorCode = WSAGetLastError();
				if (errorCode == WSAEWOULDBLOCK)
				{
					m_clientState = ClientState::CONNECTING;
				}
				else
				{
					m_clientState = ClientState::TERMINATED;
					ERROR_AND_DIE("Cannot connect the client socket");
				}

			}
			else
			{
				m_clientState = ClientState::CONNECTED;
			}
		}
		if (m_clientState == ClientState::CONNECTING)
		{
			fd_set writeSockets;
			fd_set exceptSockets;
			FD_ZERO(&writeSockets);
			FD_ZERO(&exceptSockets);
			FD_SET(m_clientSocket, &writeSockets);
			FD_SET(m_clientSocket, &exceptSockets);
			timeval waitTime = { 0 };

			int selectResult = select(0, NULL, &writeSockets, &exceptSockets, &waitTime);

			if (selectResult == SOCKET_ERROR)
			{
				sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_addr.S_un.S_addr = (m_hostAddress);
				addr.sin_port = (m_hostPort);
				int result = connect(m_clientSocket, (sockaddr*)(&addr), (int)sizeof(addr));

				if (result == SOCKET_ERROR)
				{
					int errorCode = WSAGetLastError();
					if (errorCode == WSAEWOULDBLOCK)
					{
						m_clientState = ClientState::CONNECTING;
					}
					else
					{
						m_clientState = ClientState::TERMINATED;
						DebuggerPrintf("Error: %d", errorCode);
					}
				}
			}
			else if (selectResult > 0)
			{
				if (FD_ISSET(m_clientSocket, &exceptSockets))
				{
					sockaddr_in addr;
					addr.sin_family = AF_INET;
					addr.sin_addr.S_un.S_addr = (m_hostAddress);
					addr.sin_port = (m_hostPort);
					int result = connect(m_clientSocket, (sockaddr*)(&addr), (int)sizeof(addr));
					if (result == SOCKET_ERROR)
					{
						int errorCode = WSAGetLastError();
						if (errorCode == WSAEWOULDBLOCK)
						{
							m_clientState = ClientState::CONNECTING;
						}
						else
						{
							m_clientState = ClientState::TERMINATED;
							DebuggerPrintf("Error: %d", errorCode);
						}
					}
					else
					{
						m_clientState = ClientState::CONNECTED;
					}
				}
				else if (FD_ISSET(m_clientSocket, &writeSockets))
				{
					m_clientState = ClientState::CONNECTED;
				}
				else
				{
					m_clientState = ClientState::CONNECTING;
				}
			}
		}
		if (m_clientState == ClientState::CONNECTED)
		{
			int currentBufferSize = m_config.m_sendBufferSize;
			std::memset(m_sendBuffer, 0, m_config.m_sendBufferSize);

			while (m_sendQueue.empty() == false && currentBufferSize > 0)
			{
				std::string& currentString = m_sendQueue.front();

				if (currentString.size() < currentBufferSize)
				{
					std::string sendString = currentString;
					sendString.push_back('\0');
					memcpy(m_sendBuffer + (m_config.m_sendBufferSize - currentBufferSize), sendString.c_str(), sendString.size());
					currentBufferSize -= (int)sendString.size();
					m_sendQueue.pop_front();
				}
				else
				{
					std::string sendString = currentString.substr(0, currentBufferSize);
					currentString.erase(0, sendString.size());
					memcpy(m_sendBuffer + (m_config.m_sendBufferSize - currentBufferSize), sendString.c_str(), sendString.size());
					currentBufferSize = 0;
				}
			}
			int result = 0;
			
			if (currentBufferSize < m_config.m_sendBufferSize)
			{
				result = send(m_clientSocket, m_sendBuffer, m_config.m_sendBufferSize, 0);
				currentBufferSize = m_config.m_sendBufferSize;
				if (result == SOCKET_ERROR)
				{
					int sendError = WSAGetLastError();
					if (sendError != WSAEWOULDBLOCK)
					{
						closesocket(m_clientSocket);
						m_clientSocket = INVALID_SOCKET;
						m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
						unsigned long blockingMode = 1;
						ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
						m_clientState = ClientState::DISCONNECTED;
					}
				}
			}
			

			while (true)
			{
				result = recv(m_clientSocket, m_recvBuffer, m_config.m_recvBufferSize, 0);

				if (result > 0)
				{
					m_recvQueue.append(m_recvBuffer, result);

					while (!m_recvQueue.empty() && m_recvQueue.back() == '\0')
					{
						Strings commands = SplitStringOnDelimiter(m_recvQueue, '\0');

						for (const std::string& command : commands)
						{
							if (!command.empty())
							{
								g_theConsole->Execute(command);
							}
						}
						m_recvQueue.clear();
					}
				}
				else if (result == 0)
				{
					closesocket(m_clientSocket);
					m_clientSocket = INVALID_SOCKET;
					m_clientState = ClientState::DISCONNECTED;
					break;
				}
				else if (result == SOCKET_ERROR)
				{
					int errorCode = WSAGetLastError();
					if (errorCode == WSAEWOULDBLOCK)
					{
						break;
					}
					else
					{
						closesocket(m_clientSocket);
						m_clientSocket = INVALID_SOCKET;
						m_clientState = ClientState::DISCONNECTED;
						break;
					}
				}
			}
		}
	}

	if (m_mode == NetSystemMode::SERVER)
	{
		int result = 0;

		if (m_serverState == ServerState::LISTENING)
		{
			if (m_clientSocket == INVALID_SOCKET)
			{
				m_clientSocket = accept(m_listenSocket, NULL, NULL);

				if (m_clientSocket == INVALID_SOCKET)
				{
					int errorCode = WSAGetLastError();
					if (errorCode != WSAEWOULDBLOCK)
					{
						m_serverState = ServerState::DISCONNECTED;
						ERROR_AND_DIE("clientSocket is invalid ");
					}
					m_serverState = ServerState::LISTENING;
				}
				else
				{
					unsigned long blockingMode = 1;
					ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
					m_serverState = ServerState::CONNECTED;
				}
			}
			else
			{
				m_serverState = ServerState::CONNECTED;
			}
		}
		if (m_serverState == ServerState::CONNECTED)
		{
			int currentBufferSize = m_config.m_sendBufferSize;
			std::memset(m_sendBuffer, 0, m_config.m_sendBufferSize);

			while (m_sendQueue.empty() == false && currentBufferSize > 0)
			{
				std::string& currentString = m_sendQueue.front();

				if (currentString.size() < currentBufferSize)
				{
					std::string sendString = currentString;
					sendString.push_back('\0');
					memcpy(m_sendBuffer + (m_config.m_sendBufferSize - currentBufferSize), sendString.c_str(), sendString.size());
					currentBufferSize -= (int)sendString.size();
					m_sendQueue.pop_front();
				}
				else
				{
					std::string sendString = currentString.substr(0, currentBufferSize);
					currentString.erase(0, sendString.size());
					memcpy(m_sendBuffer + (m_config.m_sendBufferSize - currentBufferSize), sendString.c_str(), sendString.size());
					currentBufferSize = 0;
				}
			}

			if (currentBufferSize < m_config.m_sendBufferSize)
			{
				result = send(m_clientSocket, m_sendBuffer, m_config.m_sendBufferSize, 0);
				if (result == SOCKET_ERROR)
				{
					int sendError = WSAGetLastError();
					if (sendError != WSAEWOULDBLOCK)
					{
						closesocket(m_clientSocket);
						m_clientSocket = INVALID_SOCKET;
						m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

						unsigned long blockingMode = 1;
						ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
						m_serverState = ServerState::LISTENING;
					}
				}
			}


			while (true)
			{
				result = recv(m_clientSocket, m_recvBuffer, m_config.m_recvBufferSize, 0);

				if (result > 0)
				{
					m_recvQueue.append(m_recvBuffer, result);

					while (!m_recvQueue.empty() && m_recvQueue.back() == '\0')
					{
						Strings commands = SplitStringOnDelimiter(m_recvQueue, '\0');

						for (const std::string& command : commands)
						{
							if (!command.empty())
							{
								EventArgs arguments;
								arguments.SetValue(std::string("InputText"), command);
								g_theEventSystem->FireEvent("echo", arguments);
							}
						}
						m_recvQueue.clear(); 
					}
				}
				else if (result == 0)
				{
					closesocket(m_clientSocket);
					m_clientSocket = INVALID_SOCKET;
					m_serverState = ServerState::LISTENING;
					break;
				}
				else if (result == SOCKET_ERROR)
				{
					int errorCode = WSAGetLastError();
					if (errorCode == WSAEWOULDBLOCK)
					{
						break;
					}
					else
					{
						closesocket(m_clientSocket);
						m_clientSocket = INVALID_SOCKET;
						m_serverState = ServerState::LISTENING;
						break;
					}
				}
			}
		}

	}
#else
	return;
#endif
}

void NetSystem::EndFrame()
{
#if !defined (ENGINE_DISABLE_NETWORK)
	// Do normal system stuff and return appropriate return values
#else
	return;
#endif
}

void NetSystem::AddStringToQueue(std::string const& stringLine)
{
	m_sendQueue.push_back(stringLine);
}

std::string NetSystem::GetCurrentRecvQueue() const
{
	return m_recvQueue;
}

NetSystemMode NetSystem::GetSystemMode() const
{
	return m_mode;
}

ClientState NetSystem::GetClientState() const
{
	return m_clientState;
}

ServerState NetSystem::GetServerState() const
{
	return m_serverState;
}
