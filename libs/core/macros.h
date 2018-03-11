#pragma once

// For compiler and platform macros
#include "core/compiler.h"

// For assert macros
#include "core/rf_assert.h"


#define RF_CONCAT_INNER(LHS, RHS) LHS ## RHS
#define RF_CONCAT(LHS, RHS) RF_CONCAT_INNER(LHS, RHS)
#define RF_STRINGIFY(X) #X

#define RF_NO_COPY(CLASS) \
	CLASS(CLASS const &) = delete; \
	CLASS & operator =(CLASS const &) = delete;
#define RF_NO_MOVE(CLASS) \
	CLASS( CLASS && ) = delete; \
	CLASS& operator =( CLASS && ) = delete;
