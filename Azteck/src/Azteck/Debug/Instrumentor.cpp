#include <azpch.h>
#include "Instrumentor.h"

namespace Azteck
{
	//----------------------------------------------------------
	Instrumentor::Instrumentor()
		: _currentSession(nullptr)
	{
	}

	void Instrumentor::beginSession(const std::string& name, const std::string& filepath)
	{
		std::lock_guard lock(_mutex);

		if (_currentSession) 
		{
			// If there is already a current session, then close it before beginning new one.
			// Subsequent profiling output meant for the original session will end up in the
			// newly opened session instead.  That's better than having badly formatted
			// profiling output.
			if (Log::getCoreLogger()) // Edge case: beginSession() might be before Log::init()
				AZ_CORE_ERROR("Instrumentor::beginSession('{0}') when session '{1}' already open.", name, _currentSession->name);

			internalEndSession();
		}

		_outputStream.open(filepath);

		if (_outputStream.is_open()) 
		{
			_currentSession = new InstrumentationSession({ name });
			writeHeader();
		}
		else if(Log::getCoreLogger()) // Edge case: BeginSession() might be before Log::Init()
		{
			AZ_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
		}
	}

	void Instrumentor::endSession()
	{
		std::lock_guard lock(_mutex);
		internalEndSession();
	}

	void Instrumentor::writeProfile(const ProfileResult& result)
	{
		std::stringstream json;

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.end - result.start) << ',';
		json << "\"name\":\"" << name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.threadID << ",";
		json << "\"ts\":" << result.start;
		json << "}";

		std::lock_guard lock(_mutex);
		if (_currentSession)
		{
			_outputStream << json.str();
			_outputStream.flush();
		}
	}

	Instrumentor& Instrumentor::get()
	{
		static Instrumentor instance;
		return instance;
	}

	void Instrumentor::writeHeader()
	{
		_outputStream << "{\"otherData\": {},\"traceEvents\":[{}";
		_outputStream.flush();
	}

	void Instrumentor::writeFooter()
	{
		_outputStream << "]}";
		_outputStream.flush();
	}

	void Instrumentor::internalEndSession()
	{
		if (_currentSession)
		{
			writeFooter();
			_outputStream.close();
			delete _currentSession;
			_currentSession = nullptr;
		}
	}

	//----------------------------------------------------------
	InstrumentationTimer::InstrumentationTimer(const char* name)
		: _name(name)
		, _stopped(false)
	{
		_startTimepoint = std::chrono::high_resolution_clock::now();
	}

	InstrumentationTimer::~InstrumentationTimer()
	{
		if (!_stopped)
			stop();
	}

	void InstrumentationTimer::stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(_startTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		Instrumentor::get().writeProfile({ _name, start, end, std::this_thread::get_id()});

		_stopped = true;
	}
}