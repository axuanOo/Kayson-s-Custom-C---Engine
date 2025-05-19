#include "Engine/Core/JobSystem.hpp"

// -----------------------------JOBSYSTEM----------------------------------
void JobSystem::StartUp()
{	
	m_workers.reserve(m_config.m_workerNumber);

	for (unsigned int workerID = 0; workerID < (unsigned int)m_config.m_workerNumber; workerID++)
	{
		JobWorkerThread* worker = new JobWorkerThread(this, workerID);
		m_workers.push_back(worker);
	}
}

void JobSystem::SetWorkerFlag(unsigned int workerID, unsigned int workerFlag)
{
	if (workerID >= 0 && workerID < (unsigned int)m_workers.size())
	{
		m_workers[workerID]->SetWorkerFlag(workerFlag);
	}
}

void JobSystem::ShutDown()
{
	m_isQuitting = true;

	for (int workerIndex = 0; workerIndex < (int)m_workers.size(); workerIndex++)
	{
		if (m_workers[workerIndex])
		{
			delete m_workers[workerIndex];
			m_workers[workerIndex] = nullptr;
		}
	}

	m_workers.clear();
}

void JobSystem::AddJobIntoDeque(Job* job)
{
	m_queuedJobsMutex.lock();
	m_queuedJobs.push_back(job);
	job->m_status = JobStatus::QUEUED;
	m_queuedJobsMutex.unlock();
}

Job* JobSystem::GetAnAvaliableJob(unsigned int jobFlag)
{
	m_queuedJobsMutex.lock();

	Job* jobNotClaimed = nullptr;

	if (m_queuedJobs .empty())
	{
		m_queuedJobsMutex.unlock();
		return jobNotClaimed;
	}

	m_jobsExecutingMutex.lock();

	for (auto it = m_queuedJobs.begin(); it != m_queuedJobs.end(); ++it)
	{
		if ((*it)->m_jobFlag == jobFlag)
		{
			jobNotClaimed = (*it);
			m_queuedJobs.erase(it);
			jobNotClaimed->m_status = JobStatus::EXECUTING;
			m_jobsExecuting.push_back(jobNotClaimed);
			break;
		}
	}

	m_jobsExecutingMutex.unlock();
	m_queuedJobsMutex.unlock();

	return jobNotClaimed;
}

void JobSystem::CompleteJob(Job* job)
{
	m_jobsExecutingMutex.lock();
	
	auto it = std::find(m_jobsExecuting.begin(), m_jobsExecuting.end(), job);

	if (it != m_jobsExecuting.end())
	{
		m_jobsExecuting.erase(it);

		m_jobsCompletedMutex.lock();
		job->m_status = JobStatus::COMPLETED;
		m_jobsCompleted.push_back(job);
		m_jobsCompletedMutex.unlock();
	}

	m_jobsExecutingMutex.unlock();
}

void JobSystem::RetrieveCompletedJob(Job* job)
{
	m_jobsCompletedMutex.lock();
	auto it = std::find(m_jobsCompleted.begin(), m_jobsCompleted.end(), job);

	if (it != m_jobsCompleted.end())
	{
		(*it)->m_status = JobStatus::RETRIEVED;
		it = m_jobsCompleted.erase(it);
	}

	m_jobsCompletedMutex.unlock();
}

void JobSystem::RetrieveAllCompletedJobs()
{
	m_jobsCompletedMutex.lock();

	for (auto it = m_jobsCompleted.begin(); it != m_jobsCompleted.end(); ++it)
	{
		(*it)->m_status = JobStatus::RETRIEVED;
	}

	m_jobsCompleted.clear();

	m_jobsCompletedMutex.unlock();
}

// -----------------------------JOBWORKER----------------------------------
JobWorkerThread::JobWorkerThread(JobSystem* systemPtr, unsigned int workerID, unsigned int workerFlag)
:m_system(systemPtr),
m_ID(workerID),
m_workerFlag(workerFlag)
{
	m_thread = new std::thread(&JobWorkerThread::ThreadMain, this);
}

JobWorkerThread::~JobWorkerThread()
{
	m_thread->join();
}

void JobWorkerThread::ThreadMain()
{
	while (!m_system->IsQuitting())
	{
		m_currentJob = m_system->GetAnAvaliableJob(m_workerFlag);

		if (m_currentJob)
		{
			m_currentJob->Execute();
			m_system->CompleteJob(m_currentJob);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}

void JobWorkerThread::SetWorkerFlag(unsigned int workerFlag)
{
	m_workerFlag = workerFlag;
}

unsigned int JobWorkerThread::GetWorkerFlag() const
{
	return m_workerFlag;
}
