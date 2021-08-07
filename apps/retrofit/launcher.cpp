
__declspec( dllimport ) int module_main( int argc, char* argv[] );

int main( int argc, char* argv[] )
{
	return module_main( argc, argv );
}
