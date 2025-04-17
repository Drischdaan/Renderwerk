#include "catch_amalgamated.hpp"

int main(const int ArgumentCount, char* Arguments[])
{
	Catch::Session Session;
	Session.configData().allowZeroTests = true;
	return Session.run(ArgumentCount, Arguments);
}
