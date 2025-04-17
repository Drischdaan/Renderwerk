#include "pch.hpp"

#include "Renderwerk/Core/Test.hpp"

#ifdef RW_KIND_CONSOLE_APP
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#endif
{
	Test();
	return 0;
}
