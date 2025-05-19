#pragma 
#include <string>
#include <cstdint>
#include <vector>
#include <deque>

struct NetSystemConfig
{
	std::string m_modeString;
	std::string m_hostAddressString;
	int			m_sendBufferSize = 2048;
	int			m_recvBufferSize = 2048;
};

enum class NetSystemMode
{
	NONE = 0,
	CLIENT,
	SERVER,
	COUNT
};

enum class ClientState
{
	CONNECTING,
	CONNECTED,
	DISCONNECTED,
	TERMINATED
};
enum class ServerState
{
	LISTENING,
	CONNECTING,
	CONNECTED,
	DISCONNECTED,
	TERMINATED
};

class NetSystem
{
public:
	NetSystem(const NetSystemConfig& config);
	~NetSystem();

	void				StartUp();
	void				ShutDown();

	void				BeginFrame();
	void				EndFrame();

	void				AddStringToQueue(std::string const& stringLine);
	std::string			GetCurrentRecvQueue() const;
	NetSystemMode		GetSystemMode() const;
	ClientState			GetClientState() const;
	ServerState			GetServerState() const;
private:

	NetSystemConfig				m_config;
	NetSystemMode				m_mode;
	ClientState					m_clientState = ClientState::DISCONNECTED;
	ServerState					m_serverState;
	uintptr_t					m_clientSocket = ~0ull;	// equivalent to INVALID_SOCKET;
	unsigned long				m_hostAddress = 0;
	unsigned short				m_hostPort = 0;
	uintptr_t					m_listenSocket = ~0ull;
	char*						m_sendBuffer;
	char*						m_recvBuffer;

	std::deque<std::string>	m_sendQueue;
	std::string					m_recvQueue;

};
