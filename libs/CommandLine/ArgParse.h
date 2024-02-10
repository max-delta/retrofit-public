#include "project.h"

#include "rftl/unordered_map"
#include "rftl/unordered_set"
#include "rftl/vector"
#include "rftl/string"


// Forwards
namespace RF::cli {
class ArgView;
}

namespace RF::cli {
///////////////////////////////////////////////////////////////////////////////

// Parses arguments in a POSIX style
// IMPORTANT: Option order is not preserved, only the order of values within
//  options and the arguments are ('--version' and '--help' may be re-ordered)
class COMMANDLINE_API ArgParse
{
	//
	// Types and constants
public:
	using Argument = rftl::string;
	using Arguments = rftl::vector<Argument>;

	using OptionName = rftl::string;
	using OptionNames = rftl::unordered_set<OptionName>;
	using OptionVal = rftl::string;
	using OptionVals = rftl::vector<OptionVal>;
	using Options = rftl::unordered_map<OptionName, OptionVals>;

	static constexpr char kOptionPrefix = '-';
	static constexpr char kOptionTerminator[] = "--";


	//
	// Public methods
public:
	ArgParse() = delete;
	ArgParse( ArgView const& args );

	OptionNames GetOptionNames() const;
	bool HasOption( OptionName const& name ) const;
	bool HasAnyOption( rftl::initializer_list<OptionName> const& names ) const;
	OptionVals GetOptionVals( OptionName const& name ) const;
	Arguments GetArguments() const;


	//
	// Private methods
private:
	void Parse( ArgView const& args );


	//
	// Public data
private:
	Arguments mArguments;
	Options mOptions;
};

///////////////////////////////////////////////////////////////////////////////
}
