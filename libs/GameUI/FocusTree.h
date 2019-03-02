#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "core/ptr/weak_ptr.h"
#include "core/ptr/unique_ptr.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FocusTree
{
	RF_NO_COPY( FocusTree );

	//
	// Forwards
public:
	struct Node;


	//
	// Types
public:
	using NodeStack = rftl::vector<WeakPtr<Node>>;
	using ConstNodeStack = rftl::vector<WeakPtr<Node const>>;
private:
	using BackingNodeStorage = rftl::vector<UniquePtr<Node>>;


	//
	// Structs
public:
	struct Node
	{
		WeakPtr<FocusTarget> mFocusTarget;

		WeakPtr<Node> mPreviousSibling;
		WeakPtr<Node> mNextSibling;

		WeakPtr<Node> mFavoredChild;
	};


	//
	// Public methods
public:
	FocusTree() = default;

	ConstNodeStack GetCurrentFocusStack() const;

	WeakPtr<Node const> GetCurrentFocus() const;

	bool IsCurrentFocus( Node const& node ) const;
	bool IsCurrentFocus( FocusTarget const& target ) const;

	static bool IsValidForFocus( Node const& node );
	static bool ShouldTraverseForFocus( Node const& node );

	WeakPtr<Node const> FindNode( FocusTarget const& target ) const;

	bool CreateNewChild( Node const& parentNode, UniquePtr<FocusTarget> const& newTarget );
	bool CreateNewSiblingAfter( WeakPtr<Node> previousNode, UniquePtr<FocusTarget> const& newTarget );
	bool CreateNewSiblingBefore( WeakPtr<Node> nextNode, UniquePtr<FocusTarget> const& newTarget );

	void TrimDeadLinks();


	//
	// Private data
private:
	WeakPtr<Node> mRootNode;

	BackingNodeStorage mBackingNodeStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}
