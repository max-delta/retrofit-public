#pragma once


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Sometimes you don't want C++ to be able to implicitly call the default
//  constructor, but you'd still like to be able to explicitly call it in code,
//  without having to make fiddle with template-hiding or shimming to privates
// EXAMPLE:
//  struct MyStruct
//  {
//    explicit MyStruct( RF::ExplicitDefaultConstruct );
//  };
//  void main()
//  {
//    MyStruct instance{ RF::ExplicitDefaultConstruct() };
//  }
struct ExplicitDefaultConstruct
{
};

///////////////////////////////////////////////////////////////////////////////
}
