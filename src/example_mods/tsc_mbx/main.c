#include <cave_story.h>
#include <mod_loader.h>
#include <string.h>
#include <windows.h>

static const char * command, * caption;

static int TSCCommand(MLHookCPURegisters * regs, void * ud) {
	char * where = CS_tsc_buffer + CS_tsc_offset;
	if (where[0] != '<')
		return 0;
	if (strncmp(where + 1, command, 3))
		return 0;
	unsigned int ofs = 4;
	while (1) {
		if (where[ofs] == ';')
			break;
		ofs++;
	}
	where[ofs] = 0;
	MessageBoxA(CS_hWnd, where + 4, caption, MB_OK);
	CS_tsc_offset += ofs;
	regs->eip = CSJ_tsc_done;
	return 1;
}

void InitMod() {
	command = ModLoader_GetSettingString("command", "MBX");
	caption = ModLoader_GetSettingString("caption", "<MBX");
	ModLoader_AddStackableHook(CSH_tsc_start, TSCCommand, (void *) 0);
}
