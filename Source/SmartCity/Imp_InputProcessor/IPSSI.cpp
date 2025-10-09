
#include "IPSSI.h"

UInputProcessorSubSystem_Imp* UInputProcessorSubSystem_Imp::GetInstance()
{
	return Cast<UInputProcessorSubSystem_Imp>(GetInstanceBase());
}
