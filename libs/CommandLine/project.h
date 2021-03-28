#pragma once

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef COMMANDLINE_EXPORTS
		#define COMMANDLINE_API __declspec( dllexport )
	#else
		#define COMMANDLINE_API __declspec( dllimport )
	#endif
#else
	#define COMMANDLINE_API
#endif
