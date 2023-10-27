#pragma once

#include <mutex>
#include <fstream>

namespace Azteck
{
	using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult
	{
		std::string name;
		FloatingPointMicroseconds start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	struct InstrumentationSession
	{
		std::string name;
	};

	//------------------------------------------------------------------
	class Instrumentor
	{
	public:
		Instrumentor(const Instrumentor&) = delete;
		Instrumentor(Instrumentor&&) = delete;

		void beginSession(const std::string& name, const std::string& filepath = "results.json");
		void endSession();

		void writeProfile(const ProfileResult& result);

		static Instrumentor& get();

	private:
		void writeHeader();
		void writeFooter();

		// Note: you must already own lock on _mutex before
		// calling internalEndSession()
		void internalEndSession();

	private:
		Instrumentor();
		~Instrumentor();

	private:
		InstrumentationSession* _currentSession;
		std::ofstream _outputStream;
		std::mutex _mutex;
	};

	//------------------------------------------------------------------
	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char* name);
		~InstrumentationTimer();

		void stop();

	private:
		const char* _name;
		std::chrono::time_point<std::chrono::steady_clock> _startTimepoint;
		bool _stopped;
	};
}

#define AZ_PROFILE 0
#if AZ_PROFILE

	// Resolve which function signature macro will be used.Note that this only
	// is resolved when the (pre)compiler starts, so the syntax highlighting
	// could mark the wrong one in your editor!
	#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
		#define AZ_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__DMC__) && (__DMC__ >= 0x810)
		#define AZ_FUNC_SIG __PRETTY_FUNCTION__
	#elif defined(__FUNCSIG__)
		#define AZ_FUNC_SIG __FUNCSIG__
	#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
		#define AZ_FUNC_SIG __FUNCTION__
	#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
		#define AZ_FUNC_SIG __FUNC__
	#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
		#define AZ_FUNC_SIG __func__
	#elif defined(__cplusplus) && (__cplusplus >= 201103)
		#define AZ_FUNC_SIG __func__
	#else
		#define AZ_FUNC_SIG "AZ_FUNC_SIG unknown!"
	#endif

	#define AZ_PROFILE_BEGIN_SESSION(name, filepath) ::Azteck::Instrumentor::get().beginSession(name, filepath)
	#define AZ_PROFILE_END_SESSION() ::Azteck::Instrumentor::get().endSession()
	#define AZ_PROFILE_SCOPE(name) ::Azteck::InstrumentationTimer timer##__LINE__(name)
	#define AZ_PROFILE_FUNCTION() AZ_PROFILE_SCOPE(AZ_FUNC_SIG)
#else
	#define AZ_PROFILE_BEGIN_SESSION(name, filepath)
	#define AZ_PROFILE_END_SESSION()
	#define AZ_PROFILE_SCOPE(name)
	#define AZ_PROFILE_FUNCTION()
#endif
