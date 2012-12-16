#include "defs.h"
#include "game.h"
#include "IwGx.h"

int main() 
{ 	
	IwGxInit();
	IwGxSetColClear(0,0,0,0);
	IwResManagerInit();
	IwGxFontInit();
	
	_NAMESPACE_(CGame)::GetInst().Init();
	_NAMESPACE_(CGame)::GetInst().Run();
	_NAMESPACE_(CGame)::GetInst().Deinit();
	_NAMESPACE_(CGame)::Destroy();

	IwGxFontTerminate();
	IwResManagerTerminate();
	IwGxTerminate();
	return 0;
} 
