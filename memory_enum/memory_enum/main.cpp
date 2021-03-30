#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <psapi.h>

using namespace std;
string getRegionTypeInfo(DWORD regionType)
{
	if (regionType == MEM_IMAGE)
		return "MEM_IMAGE";
	if (regionType == MEM_MAPPED)
		return "MEM_MAPPED";
	if (regionType == MEM_PRIVATE)
		return "MEM_PRIVATE";

	return "UNKNOWN_REGION_TYPE";
}

string getRegionStateInfo(DWORD regionState)
{
	if (regionState == MEM_COMMIT)
		return "MEM_COMMIT";
	if (regionState == MEM_RESERVE)
		return "MEM_RESERVE";
	if (regionState == MEM_FREE)
		return "MEM_FREE";

	return "UNKNOWN_PAGE_STATE";
}
string getAllocationProtectInfo(DWORD allocationProtect)
{
	if (allocationProtect & PAGE_GUARD)
		return "PAGE_GUARD";
	if (allocationProtect == PAGE_EXECUTE)
		return "PAGE_EXECUTE";
	if (allocationProtect == PAGE_EXECUTE_READ)
		return "PAGE_EXECUTE_READ";
	if (allocationProtect == PAGE_EXECUTE_READWRITE)
		return "PAGE_EXECUTE_READWRITE";
	if (allocationProtect == PAGE_EXECUTE_WRITECOPY)
		return "PAGE_EXECUTE_WRITECOPY";
	if (allocationProtect == PAGE_NOACCESS)
		return "PAGE_NOACCESS";
	if (allocationProtect == PAGE_READONLY)
		return "PAGE_READONLY";
	if (allocationProtect == PAGE_READWRITE)
		return "PAGE_READWRITE";
	if (allocationProtect == PAGE_WRITECOPY)
		return "PAGE_WRITECOPY";
	if (allocationProtect == PAGE_TARGETS_INVALID)
		return "PAGE_TARGETS_INVALID";
	if (allocationProtect == PAGE_TARGETS_NO_UPDATE)
		return "PAGE_TARGETS_NO_UPDATE";

	return "RESERVED_PAGE!";
}

int main()
{
	cout << hex << "Enter processId to show its memory: ";
	string input;
	getline(cin, input);
	DWORD processId = stoi(input);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, processId);
	if (!hProcess)
	{
		cout << "Failed to open the process!" << endl;
		return 1;
	}

	MEMORY_BASIC_INFORMATION currentInfo{};
	uintptr_t currentAddress = 0;
	size_t status = 0;
	cout << "Starting!" << endl;
	ofstream fileStream("memory.txt");
	do
	{
		status = VirtualQueryEx(hProcess, (void*)currentAddress, &currentInfo, sizeof(MEMORY_BASIC_INFORMATION));
		if (currentInfo.State != MEM_FREE)
		{
			fileStream << hex << "------------------" << endl;
			fileStream << "AllocationBase: " << (uintptr_t)currentInfo.AllocationBase << endl;
			fileStream << "AllocationProtect: " << getAllocationProtectInfo(currentInfo.AllocationProtect) << endl;
			fileStream << "BaseAddress: " << (uintptr_t)currentInfo.BaseAddress << endl;
			fileStream << "Protect: " << getAllocationProtectInfo(currentInfo.Protect) << endl;
			fileStream << "RegionSize: " << dec <<  currentInfo.RegionSize/4096 << " pages. (" << currentInfo.RegionSize/1024 << "kb)" << hex << endl;
			fileStream << "State: " << getRegionStateInfo(currentInfo.State) << endl;
			fileStream << "Type: " << getRegionTypeInfo(currentInfo.Type) << endl;
			if (currentInfo.Type == MEM_IMAGE)
			{
				char fileName[MAX_PATH]{};
				if (!GetMappedFileNameA(hProcess, currentInfo.BaseAddress, fileName, MAX_PATH))
				{
					fileStream << "Image can not be found, ANOMALY! " << endl;
				}
				else
				{
					fileStream << "Image: " << fileName << endl;
				}
			}
			
		}
		currentAddress = (uintptr_t)currentInfo.BaseAddress + /*(currentInfo.Type == MEM_IMAGE ? 0x1000 :*/ currentInfo.RegionSize;
	} while (status);

	cout << "Finished!" << endl;
	fileStream.close();
	return 0;
}