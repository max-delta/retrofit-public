#pragma once

// Performs macro expansions that would not normally be performed when using
//  just the normal preprocessor syntax
#define RF_CONCAT_INNER( LHS, RHS ) LHS##RHS
#define RF_CONCAT( LHS, RHS ) RF_CONCAT_INNER( LHS, RHS )
#define RF_STRINGIFY_INNER( X ) #X
#define RF_STRINGIFY( X ) RF_STRINGIFY_INNER( X )
