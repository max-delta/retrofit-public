// Don't check for these! This machinery may change at a later date
#define RF_ALLOW 1
#define RF_FORBID 0

#define RF_IS_ALLOWED( MACRO ) ( ( MACRO ) == RF_ALLOW )
#define RF_IS_FORBIDDEN( MACRO ) ( ( MACRO ) == RF_FORBID )
