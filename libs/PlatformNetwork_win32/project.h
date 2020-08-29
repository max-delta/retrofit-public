#pragma once

#ifndef RF_NO_SHARED_EXPORTS
	#ifdef PLATFORMNETWORK_EXPORTS
		#define PLATFORMNETWORK_API __declspec( dllexport )
	#else
		#define PLATFORMNETWORK_API __declspec( dllimport )
	#endif
#else
	#define PLATFORMNETWORK_API
#endif
