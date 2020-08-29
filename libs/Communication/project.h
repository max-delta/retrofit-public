#pragma once

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef COMMUNICATION_EXPORTS
		#define COMMUNICATION_API __declspec( dllexport )
	#else
		#define COMMUNICATION_API __declspec( dllimport )
	#endif
#else
	#define COMMUNICATION_API
#endif
