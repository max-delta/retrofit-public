#include "project.h"

#include "CommandLine/CommandLineFwd.h"

#include "rftl/extension/string_hash.h"
#include "rftl/unordered_map"
#include "rftl/unordered_set"
#include "rftl/vector"
#include "rftl/string"
#include "rftl/string_view"


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
	using OptionNames = rftl::unordered_set<OptionName, rftl::string_hash, rftl::equal_to<>>;
	using OptionVal = rftl::string;
	using OptionVals = rftl::vector<OptionVal>;
	using Options = rftl::unordered_map<OptionName, OptionVals, rftl::string_hash, rftl::equal_to<>>;

	static constexpr char kOptionPrefix = '-';
	static constexpr char kOptionTerminator[] = "--";


	//
	// Public methods
public:
	ArgParse() = delete;
	ArgParse( ArgView const& args );

	OptionNames GetOptionNames() const;
	bool HasOption( rftl::string_view name ) const;
	bool HasAnyOption( rftl::initializer_list<rftl::string_view> const& names ) const;
	OptionVals GetOptionVals( rftl::string_view name ) const;
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
