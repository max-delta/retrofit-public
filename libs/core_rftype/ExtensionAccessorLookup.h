#pragma once

#include "core_reflect/ClassInfo.h"


namespace RF::rftype::extensions {
///////////////////////////////////////////////////////////////////////////////

// This just provides some useful typedefs needed for properly building an
//  extension accessor
struct AccessorTemplate
{
	using ExtensionAccessor = ::RF::reflect::ExtensionAccessor;
	using RootInst = ExtensionAccessor::RootInst;
	using RootConstInst = ExtensionAccessor::RootConstInst;
	using ClassInst = ExtensionAccessor::ClassInst;
	using ExtenInst = ExtensionAccessor::ExtenInst;
	using UntypedInst = ExtensionAccessor::UntypedInst;
	using UntypedConstInst = ExtensionAccessor::UntypedConstInst;

	using VariableTypeInfo = ::RF::reflect::VariableTypeInfo;
	using Value = ::RF::reflect::Value;
};

// By default, all types do not have extension support, and will need to be
//  specialized to participate in RFType operations if they aren't supported
//  as first-class types (Examples of first-class typas are primitives or
//  objects with ClassInfo metadata)
// NOTE: For an example of how to specialize for a new accessor, look at the
//  extensions that come with RFType, such as the 'stl_extensions'
// IMPORTANT: It is strongly discouraged to make any extension accessor
//  available to external modules (such as by dll-export). Doing so could cause
//  corruption due to differences in compilation settings, where an accessor
//  makes improper assumptions about the memory layout of another module's
//  structures. It is instead recommended to keep all extension accessors in
//  headers, or build artifacts that don't cross module boundaries (such as
//  static libs). The code duplication in the resulting binaries should be
//  minimal in most cases.
template<class CLASS>
struct Accessor final : private AccessorTemplate
{
	using AccessedType = CLASS;
	static constexpr bool kExists = false;
	static ExtensionAccessor Get();
};

///////////////////////////////////////////////////////////////////////////////
}
