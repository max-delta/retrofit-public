#include "stdafx.h"
#include "StateLogging.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/components/Meta.h"

#include "core_math/math_clamps.h"
#include "core_state/VariableIdentifier.h"
#include "core_component/TypedObjectRef.h"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////

template<>
void WriteContextString( state::VariableIdentifier const& context, Utf8LogContextBuffer& buffer )
{
	size_t bufferOffset = 0;
	size_t const maxBufferOffset = buffer.size();

	size_t const numElements = context.NumElements();
	for( size_t i = 0; i < numElements; i++ )
	{
		state::VariableIdentifier::Element const& element = context.GetElement( i );

		if( bufferOffset >= maxBufferOffset )
		{
			break;
		}
		if( i != 0 )
		{
			buffer[bufferOffset] = '.';
			bufferOffset++;
		}
		if( bufferOffset >= maxBufferOffset )
		{
			break;
		}

		size_t const bytesRemaining = maxBufferOffset - bufferOffset;
		size_t const bytesToWrite = math::Min( bytesRemaining, element.size() );
		memcpy( &buffer[bufferOffset], element.data(), bytesToWrite );
		bufferOffset += bytesToWrite;
	}
}



template<>
void WriteContextString( cc::state::ObjectRef const& context, Utf8LogContextBuffer& buffer )
{
	using namespace cc::state;
	ComponentInstanceRefT<comp::Meta> const meta = context.GetComponentInstanceT<comp::Meta>();
	if( meta == nullptr )
	{
		return WriteContextString<char const*>( "!!NoMeta!!", buffer );
	}

	return WriteContextString( meta->mIdentifier, buffer );
}



template<>
void WriteContextString( cc::state::MutableObjectRef const& context, Utf8LogContextBuffer& buffer )
{
	return WriteContextString<cc::state::ObjectRef>( context, buffer );
}

///////////////////////////////////////////////////////////////////////////////
}
