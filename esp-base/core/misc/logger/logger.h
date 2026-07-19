#pragma once

#include <chrono>

class c_logger {
private:
	__forceinline void print_timestamp( ) {
		auto now = std::chrono::system_clock::now( );
		std::time_t time = std::chrono::system_clock::to_time_t( now );
		tm local_tm;
		localtime_s( &local_tm, &time );

		printf( "\x1b[38;2;31;100;176m[\x1b[38;2;34;105;174m%02d\x1b[38;2;37;110;172m/\x1b[38;2;40;115;170m%02d\x1b[38;2;43;120;168m/\x1b[38;2;46;125;166m%04d\x1b[38;2;49;130;164m \x1b[38;2;52;135;162m%02d\x1b[38;2;50;130;160m:\x1b[38;2;50;130;160m%02d\x1b[38;2;50;130;160m:\x1b[38;2;50;130;160m%02d]\x1b[0m",
			local_tm.tm_mon + 1,
			local_tm.tm_mday,
			local_tm.tm_year + 1900,
			local_tm.tm_hour,
			local_tm.tm_min,
			local_tm.tm_sec );
	}

	template<typename... Args>
	__forceinline void print_message( const char* format, Args... args ) {
		printf( "\x1b[37m" );
		printf( format, args... );
		printf( "\x1b[0m\n" );
	}

public:
	template<typename... Args>
	__forceinline void info( const char* format, Args... args ) {
		print_timestamp( );
		printf( "\x1b[38;2;60;120;210m[info]\x1b[0m " );
		print_message( format, args... );
	}

	template<typename... Args>
	__forceinline void warn( const char* format, Args... args ) {
		print_timestamp( );
		printf( "\x1b[38;2;210;170;60m[warn]\x1b[0m " );
		print_message( format, args... );
	}

	template<typename... Args>
	__forceinline void error( const char* format, Args... args ) {
		print_timestamp( );
		printf( "\x1b[38;2;210;60;60m[error]\x1b[0m " );
		print_message( format, args... );
	}

	template<typename... Args>
	__forceinline void debug( const char* format, Args... args ) {
		print_timestamp( );
		printf( "\x1b[38;2;50;150;250m[debug]\x1b[0m " );
		print_message( format, args... );
	}
};
inline auto logger = std::make_shared<c_logger>( );