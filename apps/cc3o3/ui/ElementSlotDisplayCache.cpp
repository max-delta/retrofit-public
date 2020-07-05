#include "stdafx.h"
#include "ElementSlotDisplayCache.h"

#include "cc3o3/elements/ElementDesc.h"
#include "cc3o3/elements/IdentifierUtils.h"


namespace RF::cc::ui {
///////////////////////////////////////////////////////////////////////////////

void ElementSlotDisplayCache::UpdateFromDesc( element::ElementDesc const& desc, bool includeStrings )
{
	if( desc.mIdentifier == element::kInvalidElementIdentifier )
	{
		*this = {};
		if( includeStrings )
		{
			mName = "INVALID";
		}
		mTilesetIndex = ElementTilesetIndex::ReservedHashed;
		return;
	}

	if( includeStrings )
	{
		element::ElementName const name = element::GetElementName( desc.mIdentifier );
		mName.assign( name.begin(), name.end() );
	}
	else
	{
		mName.clear();
	}

	// TODO: Move to ui color/innate helper functions
	static constexpr auto getTilesetIndex = []( element::InnateIdentifier identifier ) -> ElementTilesetIndex //
	{
		using namespace element;
		if( identifier == InnateString{ 'w', 'h', 't' } )
			return ElementTilesetIndex::White;
		if( identifier == InnateString{ 'b', 'l', 'k' } )
			return ElementTilesetIndex::Black;
		if( identifier == InnateString{ 'r', 'e', 'd' } )
			return ElementTilesetIndex::Red;
		if( identifier == InnateString{ 'b', 'l', 'u' } )
			return ElementTilesetIndex::Blue;
		if( identifier == InnateString{ 'y', 'e', 'l' } )
			return ElementTilesetIndex::Yellow;
		if( identifier == InnateString{ 'g', 'r', 'n' } )
			return ElementTilesetIndex::Green;
		if( identifier == InnateString{ 't', 'e', 'k' } )
			return ElementTilesetIndex::Material;
		return ElementTilesetIndex::ReservedHashed;
	};

	mTilesetIndex = getTilesetIndex( desc.mInnate );
}



void ElementSlotDisplayCache::Unallocate()
{
	mName.clear();
	mTilesetIndex = ElementTilesetIndex::Unallocated;
}



void ElementSlotDisplayCache::Darken()
{
	// TODO: Move to ui color/innate helper functions
	bool const isElement =
		mTilesetIndex >= ElementTilesetIndex::ElementStart &&
		mTilesetIndex <= ElementTilesetIndex::ElementEnd;
	if( isElement )
	{
		rftl::underlying_type<ElementTilesetIndex>::type const offset =
			math::enum_bitcast( mTilesetIndex ) -
			math::enum_bitcast( ElementTilesetIndex::ElementStart );
		bool const isLight = ( offset % 3 ) == 0;
		if( isLight )
		{
			mTilesetIndex =
				math::enum_bitcast<ElementTilesetIndex>(
					math::enum_bitcast( mTilesetIndex ) +
					kElementTilesetOffsetDark );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}
