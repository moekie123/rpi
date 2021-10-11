#pragma once

#include <string>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#define TRACE( ... ) logger::trace( __PRETTY_FUNCTION__, ##__VA_ARGS__ )
#define DEBUG( ... ) logger::debug( __PRETTY_FUNCTION__, ##__VA_ARGS__ )
#define INFO( ... ) logger::info( __PRETTY_FUNCTION__, ##__VA_ARGS__ )
#define WARN( ... ) logger::warning( __PRETTY_FUNCTION__, ##__VA_ARGS__ )
#define ERROR( ... ) logger::error( __PRETTY_FUNCTION__, ##__VA_ARGS__ )
#define CRITICAL( ... ) logger::critical( __PRETTY_FUNCTION__, ##__VA_ARGS__ )

namespace logger
{
	template<typename... Args>
	inline void trace( const std::string func, Args &&...args)
	{
		SPDLOG_TRACE( "[{}] {}", func, args... );
	}

	template<typename... Args>
	inline void debug( const std::string func, Args &&...args)
	{
		SPDLOG_DEBUG( "[{}] {}", func, args... );
	}

	template<typename... Args>
	inline void info( const std::string func, Args &&...args)
	{
		SPDLOG_INFO( "[{}] {}", func, args... );
	}

	template<typename... Args>
	inline void warning( const std::string func, Args &&...args)
	{
		SPDLOG_WARN( "[{}] {}", func, args... );
	}

	template<typename... Args>
	inline void error( const std::string func, Args &&...args)
	{
		SPDLOG_ERROR( "[{}] {}", func, args... );
	}

	template<typename... Args>
	inline void critical( const std::string func, Args &&...args)
	{
		SPDLOG_CRITICAL( "[{}] {}", func, args... );
	}
}
