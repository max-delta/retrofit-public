#pragma once
#include "project.h"

#include "GameUI/UIFwd.h"

#include "core/ptr/weak_ptr.h"
#include "core/ptr/unique_ptr.h"


namespace RF { namespace ui {
///////////////////////////////////////////////////////////////////////////////

struct GAMEUI_API FocusTreeNode
{
	WeakPtr<FocusTarget> mFocusTarget;

	WeakPtr<FocusTreeNode> mPreviousSibling;
	WeakPtr<FocusTreeNode> mNextSibling;

	WeakPtr<FocusTreeNode> mFavoredChild;
};

///////////////////////////////////////////////////////////////////////////////

class GAMEUI_API FocusTree
{
	RF_NO_COPY( FocusTree );

	//
	// Types
public:
	using NodeStack = rftl::vector<WeakPtr<FocusTreeNode>>;
	using ConstNodeStack = rftl::vector<WeakPtr<FocusTreeNode const>>;
private:
	using BackingNodeStorage = rftl::vector<UniquePtr<FocusTreeNode>>;


	//
	// Public methods
public:
	FocusTree();

	FocusTreeNode const& GetRootNode() const;

	ConstNodeStack GetCurrentFocusStack() const;

	WeakPtr<FocusTreeNode const> GetCurrentFocus() const;

	bool IsCurrentFocus( FocusTreeNode const& node ) const;
	bool IsCurrentFocus( FocusTarget const& target ) const;

	static bool IsValidForFocus( FocusTreeNode const& node );
	static bool ShouldTraverseForFocus( FocusTreeNode const& node );

	WeakPtr<FocusTreeNode const> FindNode( FocusTarget const& target ) const;

	WeakPtr<FocusTreeNode> CreateNewChild( FocusTreeNode const& parentNode, UniquePtr<FocusTarget> const& newTarget );
	WeakPtr<FocusTreeNode> CreateNewSiblingAfter( WeakPtr<FocusTreeNode> previousNode, UniquePtr<FocusTarget> const& newTarget );
	WeakPtr<FocusTreeNode> CreateNewSiblingBefore( WeakPtr<FocusTreeNode> nextNode, UniquePtr<FocusTarget> const& newTarget );

	void TrimDeadLinks();


	//
	// Private methods
private:
	static bool OnRootFocusEvent( void* userData, FocusEvent const& focusEvent );


	//
	// Private data
private:
	UniquePtr<FocusTreeNode> mRootNode;
	UniquePtr<FocusTarget> mRootFocus;

	BackingNodeStorage mBackingNodeStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}
