typedef void(_stdcall* RTLPROCESSCRITICAL) (IN BYTE NewValue, OUT PBYTE OldValue, IN BYTE IsWinlogon);

WINBOOL EnablePrivs(LPCWSTR lpprivs) {
	HANDLE htk;
	LUID luid;
	TOKEN_PRIVILEGES token;
	RtlZeroMemory(&token, sizeof(token));
	
	if (!OpenProcessToken(GetCurrentProcess(), (TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY), &htk)) return false;
	if (!LookupPrivilegeValueW(NULL, lpprivs, &luid)) {
		CloseHandle(htk);
		return false;
	}
	
	token.PrivilegeCount = 1;
	token.Privileges[0].Luid = luid;
	token.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	
	BOOL bret = AdjustTokenPrivileges(htk, false, &token, sizeof(token), NULL, NULL);
	CloseHandle(htk);
	return bret;
}

WINBOOL SetCritProcess(void) {
	HANDLE hlib;
	RTLPROCESSCRITICAL SetCritiProcess;
	
	hlib = LoadLibraryW(L"ntdll.dll");
	if (hlib != NULL) {
		EnablePrivs(L"SeDebugPrivilege");
		(SetCritiProcess) = (RTLPROCESSCRITICAL)GetProcAddress((HINSTANCE)hlib, "RtlSetProcessIsCritical");
		if (!SetCritiProcess) return 0;
		SetCritiProcess(1, 0, 0);
		return 1;
	}
	else return 0;
}
