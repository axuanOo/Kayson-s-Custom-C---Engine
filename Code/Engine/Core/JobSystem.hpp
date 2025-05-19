#pragma once
#include <deque>
#include <vector>
#include <thread>
#include <mutex>
enum class JobStatus
{
	NEW,
	QUEUED,
	EXECUTING,
	COMPLETED,
	RETRIEVED,
	COUNT
};


class Job // Abstract class and must be override by the game code
{
public:
	Job(unsigned int flag = 0) : m_jobFlag(flag) {};
	virtual ~Job() {};
	virtual void Execute() = 0;

	JobStatus					m_status = JobStatus::NEW;
	unsigned int				m_jobFlag = 0; // 0 means this job can be done by any worker
};

struct JobSystemConfig
{
	unsigned int m_workerNumber = 0; // 0 means only 1 thread
};

class JobWorkerThread;
class JobSystem
{
public:
	JobSystem() = delete;
	JobSystem(JobSystemConfig config) : m_config(config){};
	~JobSystem() {};

	void							StartUp();
	void							SetWorkerFlag(unsigned int workerID, unsigned int workerFlag);
	void							ShutDown();

	void							AddJobIntoDeque(Job* job);
	Job*							GetAnAvaliableJob(unsigned int jobFlag = 0);
	void							CompleteJob(Job* job);
	void							RetrieveCompletedJob(Job* job);
	void							RetrieveAllCompletedJobs();

	bool							IsQuitting() { return m_isQuitting; }
private:
	JobSystemConfig					m_config;
	bool							m_isQuitting = false;
	std::vector<JobWorkerThread*>	m_workers;

	std::mutex						m_queuedJobsMutex;
	std::mutex						m_jobsExecutingMutex;
	std::mutex						m_jobsCompletedMutex;

	std::deque<Job*>				m_queuedJobs;
	std::deque<Job*>				m_jobsExecuting;
	std::deque<Job*>				m_jobsCompleted;
};

class JobWorkerThread 
{
public:
	JobWorkerThread() = delete;
	JobWorkerThread(JobSystem* systemPtr, unsigned int workerID, unsigned int workerFlag = 0);
	~JobWorkerThread();

	void							ThreadMain();

	void							SetWorkerFlag(unsigned int workerFlag);
	unsigned int					GetWorkerFlag() const;
private:
	unsigned int					m_workerFlag = 0;	// 0 means can be used for all kinds of job
	JobSystem*						m_system;
	unsigned int					m_ID;
	std::thread*					m_thread;
	Job*							m_currentJob = nullptr;
};