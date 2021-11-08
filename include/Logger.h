#pragma once

#include <spdlog/spdlog.h>

namespace logger
{
	template<typename... Args>
	inline void trace( Args &&...args )
	{
		spdlog::trace(  args... );
	}

	template<typename... Args>
	inline void debug( Args &&...args )
	{
		spdlog::debug( args... );
	}

	template<typename... Args>
	inline void info( Args &&...args )
	{
		spdlog::info( args... );
	}

	template<typename... Args>
	inline void warning( Args &&...args )
	{
		spdlog::warn( args... );
	}

	template<typename... Args>
	inline void error( Args &&...args )
	{
		spdlog::error( args... );
	}

	template<typename... Args>
	inline void critical(  Args &&...args )
	{
		spdlog::critical( args... );
	}
}
