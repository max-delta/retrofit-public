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
	bool IsInCurrentFocusStack( FocusTreeNode const& node ) const;
	bool IsInCurrentFocusStack( FocusTarget const& target ) const;
	bool IsInCurrentFocusStack( ContainerID const& containerID ) const;

	static bool IsValidForFocus( FocusTreeNode const& node );
	static bool ShouldTraverseForFocus( FocusTreeNode const& node );

	WeakPtr<FocusTreeNode const> FindNode( FocusTarget const& target ) const;

	WeakPtr<FocusTreeNode> CreateNewChild( FocusTreeNode const& parentNode, UniquePtr<FocusTarget> const& newTarget );
	WeakPtr<FocusTreeNode> CreateNewSiblingAfter( WeakPtr<FocusTreeNode> previousNode, UniquePtr<FocusTarget> const& newTarget );
	WeakPtr<FocusTreeNode> CreateNewSiblingBefore( WeakPtr<FocusTreeNode> nextNode, UniquePtr<FocusTarget> const& newTarget );

	bool HasImmediateChild( FocusTreeNode const& parentNode, WeakPtr<FocusTreeNode const> queryNode ) const;

	bool CycleRootFocusToNextChild( bool wrapIfLastChild );
	bool CycleRootFocusToPreviousChild( bool wrapIfLastChild );
	bool SetRootFocusToSpecificChild( WeakPtr<FocusTreeNode> targetNode );

	bool CycleFocusToNextChild( FocusTreeNode& parentNode, bool wrapIfLastChild );
	bool CycleFocusToPreviousChild( FocusTreeNode& parentNode, bool wrapIfLastChild );
	bool CycleFocusToFirstChild( FocusTreeNode& parentNode );
	bool CycleFocusToLastChild( FocusTreeNode& parentNode );
	bool SetFocusToSpecificChild( FocusTreeNode& parentNode, WeakPtr<FocusTreeNode> targetNode );

	void TrimDeadLinks();


	//
	// Private methods
private:
	static bool OnRootFocusEvent( UIContext& context, void* userData, FocusEvent const& focusEvent );


	//
	// Private data
private:
	UniquePtr<FocusTreeNode> mRootNode;
	UniquePtr<FocusTarget> mRootFocus;

	BackingNodeStorage mBackingNodeStorage;
};

///////////////////////////////////////////////////////////////////////////////
}}
