#include "NMSE_Libs\Hooking.h"
#include "NMSE_Libs\Steam.h"
#include "NMSE_Libs\VersionControl.h"

// Have to give large credit to the F4SE/SKSE/OBSE etc.. crew. This first pre-alpha is VERY LARGELY based off their SE's.
#define DEBUG 0
//275850 -- game id
int main(int argc, char* argv) {
	const char* dllName = "NMSE Core.dll";
	std::string prgmName = "NMS.exe";
	std::string curPath = RunTimePath();

	std::cout << "Using runtime: " << curPath << "\n";
	bool steam;
	if (CheckFile(curPath+"\\steam_api64.dll")){
		steam = true;
		std::cout << "Using Steam Version\n";
		if (!CheckSteam()) {
			std::cout << "Steam not open... Launching\n";
			LaunchSteam();
		}
	}
	else{
		steam = false;
		std::cout << "Using GOG Version\n";
	}


	std::cout << "Booting NMS...\n";

	std::string exe = curPath + "\\" + prgmName;

	//ValidateVersion(exe); -- waiting for VC implementation on HG's side :\
	

	if (steam){
		const char* nmsId = "275850";
		SetEnvironmentVariable("SteamGameId", nmsId);
		SetEnvironmentVariable("SteamAppID", nmsId);
	}

	STARTUPINFO startup = { 0 };
	PROCESS_INFORMATION nmsProc = { 0 };


	if (!CreateProcess(exe.c_str(), NULL, NULL, NULL, false,
		CREATE_SUSPENDED, NULL, NULL, &startup, &nmsProc)) {
		std::cout << "Failed to start NMS: " << GetLastError() << std::endl;
		std::cout << "If 740 please try running as Admin\n";
	}
#if DEBUG
	std::cout << "HOOK BASE: " << std::hex << GetModuleHandle(NULL) << std::endl;
	MessageBox(0, "APP SUSPENDED, HOOK AND PRESS OK", "CUSTOM NMSE", MB_OK | MB_ICONWARNING);
#endif
	//inject the dll
	std::string NMSEsteam = curPath + "\\NMSE_steam.dll";
	bool isInjected = InjectDLLThread(&nmsProc, NMSEsteam.c_str(), true, false);
	if (isInjected) {
		std::cout << "Loader injected...\n";
	}
	else {
		std::cout << "Loader Injection Failed\n";
	}

	
	Sleep(1000);
	std::cout << "Resuming thread...\n";
	if (!ResumeThread(nmsProc.hThread)) {
		std::cout << "Thread resume failed (ERR): " << GetLastError() << std::endl;
	}
	std::string rtp(RunTimePath()); //save some func calls
	if (CheckFile(rtp + "\\opengl32.dll")){ //stat doesn't seem to have a quick and easy way to implment non case-sensitivity
		//so this hack will have to do
		InjectDLLThread(&nmsProc, std::string(RunTimePath() + "\\opengl32.dll").c_str(), true, false);
	}
	else if (CheckFile(rtp + "\\Opengl32.dll")){ //stat doesn't seem to have a quick and easy way to implment non case-sensitivity
		//so this hack will have to do
		InjectDLLThread(&nmsProc, std::string(rtp + "\\Opengl32.dll").c_str(), true, false);
	}
	else if (CheckFile(rtp + "\\OPENGL32.dll")){ //stat doesn't seem to have a quick and easy way to implment non case-sensitivity
		//so this hack will have to do
		InjectDLLThread(&nmsProc, std::string(rtp + "\\OPENGL32.dll").c_str(), true, false);
	}
	else if (CheckFile(rtp + "\\OpenGL32.dll")){ //stat doesn't seem to have a quick and easy way to implment non case-sensitivity
		//so this hack will have to do
		InjectDLLThread(&nmsProc, std::string(rtp + "\\OpenGL32.dll").c_str(), true, false);
	}

	if (CheckFile(rtp + "\\xinput9_1_0.dll")){
		InjectDLLThread(&nmsProc, std::string(rtp + "\\xinput9_1_0.dll").c_str(), true, false);
	}
	if (CheckFile(rtp + "\\opengl32.dll")){ //stat doesn't seem to have a quick and easy way to implment non case-sensitivity
		//so this hack will have to do
		std::cout << "YOU HAVE THE OPENGL32 DLL INSTALLED\n";
	}
	else{
		std::cout << "No opengl32.dll Found! \n";
	}
	
	std::cout << "\nProgram Injected Everything Succesfully!\n";
	std::cout << "Please wait for this to close before worrying :)\n";
	CloseHandle(nmsProc.hProcess);
	CloseHandle(nmsProc.hThread);
	Sleep(25000);
	return 0;
}
