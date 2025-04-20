#include "catch_amalgamated.hpp"

int main(const int ArgumentCount, char* Arguments[])
{
	Catch::Session Session;
	Session.configData().waitForKeypress = Catch::WaitForKeypress::Never;
	Session.configData().verbosity = Catch::Verbosity::High;
	return Session.run(ArgumentCount, Arguments);
}
