#include "pch.hpp"

#ifdef RW_KIND_CONSOLE_APP
int main()
#else
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#endif
{
	return 0;
}
