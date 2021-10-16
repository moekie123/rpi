#pragma once

#include <string>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

namespace logger
{
	template<typename... Args>
	inline void trace( Args &&...args )
	{
		SPDLOG_TRACE(  args... );
	}

	template<typename... Args>
	inline void debug( Args &&...args )
	{
		SPDLOG_DEBUG( args... );
	}

	template<typename... Args>
	inline void info( Args &&...args )
	{
		SPDLOG_INFO( args... );
	}

	template<typename... Args>
	inline void warning( Args &&...args )
	{
		SPDLOG_WARN( args... );
	}

	template<typename... Args>
	inline void error( Args &&...args )
	{
		SPDLOG_ERROR( args... );
	}

	template<typename... Args>
	inline void critical(  Args &&...args )
	{
		SPDLOG_CRITICAL( args... );
	}
}
