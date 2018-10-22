#include <cave_story.h>
#include <mod_loader.h>
#include <string.h>
#include <windows.h>

static int TSCCommand(MLHookCPURegisters * regs, void * ud)
{
	(void)ud;

	char * where = CS_tsc_buffer + CS_tsc_offset;
	if (where[0] != '<')
		return 0;
	if (strncmp(where + 1, "NOD", 3))
		return 0;
	CS_tsc_offset += 4;
	regs->eip = CSJ_tsc_done;
	return 1;
}

void InitMod() {
	ModLoader_AddStackableHook(CSH_tsc_start, TSCCommand, (void *) 0);
}
