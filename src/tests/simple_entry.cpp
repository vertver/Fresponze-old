#include <windows.h>
#include <stdio.h>
#include "Fresponze.h"

IFresponze* pFresponze = nullptr;

int main()
{


	if (FrInitializeInstance((void**)&pFresponze) != 0) return -1;
	pFresponze->GetHardwareInterface(eEndpointWASAPIType, nullptr, nullptr);
	return 0;
}
