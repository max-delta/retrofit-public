#pragma once


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// There are cases when you need factory pattern, either as a function pointer
//  or a callable instance
template<typename T>
struct DefaultConstruct
{
	static T Create()
	{
		return T();
	}

	T operator()()
	{
		return Create();
	}
};

///////////////////////////////////////////////////////////////////////////////
}
