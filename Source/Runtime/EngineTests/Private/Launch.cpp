#include "catch_amalgamated.hpp"

int main(const int ArgumentCount, char* Arguments[])
{
	Catch::Session Session;
	Session.configData().allowZeroTests = true;
	Session.configData().showDurations = Catch::ShowDurations::Always;
	return Session.run(ArgumentCount, Arguments);
}
