#include "project.h"

#include "core/macros.h"


namespace RF::cli {
///////////////////////////////////////////////////////////////////////////////

// IMPORTANT: No promises are made as to the lifetime of the strings provided,
//  though a non-disgusting program should avoid modifying them
// IMPORTANT: C++17 standard doesn't seem to indicate encoding, but that the
//  strings are multi-byte (non-ASCII, likely UTF-8)
// SEE: ISO N4713 C++ 6.8.3.1.2
class COMMANDLINE_API ArgView
{
	RF_DEFAULT_COPY( ArgView );

	//
	// Types and constants
public:
	RF_TODO_ANNOTATION( "Update to char8_t in C++20" );
	using value_type = char const* const;

	//
	// Public methods
public:
	ArgView() = default;
	ArgView( int argc, char* argv[] );
	ArgView( size_t argc, value_type argv[] );
	~ArgView() = default;

	value_type* begin() const;
	value_type* end() const;


	//
	// Private data
private:
	size_t mArgC = 0;
	value_type* mArgV = nullptr;
};

///////////////////////////////////////////////////////////////////////////////
}
