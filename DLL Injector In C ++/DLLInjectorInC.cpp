// DLLInjectorInC.cpp : Defines the entry point for the console application.

#define MAX_PATH 1000
#include "stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>

using namespace std;

bool injectdll(DWORD processid, char* dllpath)
{
	bool returnit = false;
	cout << "Attaching the process ............" << endl;
	HANDLE handletargetprocess = OpenProcess(PROCESS_ALL_ACCESS, false,processid);
	if (handletargetprocess)
	{
		cout << "Attached the process...." << endl;
		cout << "Allocating  the memory ........" << endl;
		LPVOID dllpathmemoryaddress = VirtualAllocEx(handletargetprocess, 0, strlen(dllpath), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		if (dllpathmemoryaddress)
		{
			cout << "Allocated the Memory ....." << endl;
			cout << "Writting the process Memory .......... " << endl;
			if (WriteProcessMemory(handletargetprocess, dllpathmemoryaddress, dllpath, strlen(dllpath), NULL))
			{
				cout << "Written the process Memory...." << endl;
				cout << "Loading the Library ......" << endl;
			LPVOID	loadlibraryaddress = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

			cout << "library loaded........" << endl;
			cout << "Creating the remote thread for dll....." << endl;
			HANDLE remotethread = CreateRemoteThread(handletargetprocess, NULL, 0, (LPTHREAD_START_ROUTINE)loadlibraryaddress, dllpathmemoryaddress, 0, NULL);
			if (remotethread)
			{
				returnit = true;
			}
			}
		}

	}
	CloseHandle(handletargetprocess);
	cout << "Created the remote thread ...." << endl;
	cout << "Injected the DLL (" << dllpath << ") to process (" << processid << ")....." << endl;
	cout << "DLL INJECTED..." << endl;
	return returnit;
}

DWORD MyGetProcessId(LPCTSTR ProcessName) 
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt))
	{

		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName))
			{
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap);
	cout << "Sorry unable to find the id make sure that process is running.............." << endl;
	cout << "--------------------------------------------------------------------------------" << endl;
	return 0; 
}
LPCTSTR SzToLPCTSTR(char* szString)
{
	LPTSTR  lpszRet;
	size_t  size = strlen(szString) + 1;
	 
	lpszRet = (LPTSTR)malloc(MAX_PATH);
	mbstowcs_s(NULL, lpszRet, size, szString, _TRUNCATE);

	return lpszRet;
}

int main()
{
	char    proc[MAX_PATH];
	char*   szDllPath = (char*)malloc(MAX_PATH);
	LPCTSTR Process = NULL;
	cout << "----------------------Welcome to our Dll Injector----------------------" << endl;
	while (true)
	{
		cout << "Do you want to inject a dll in a process (y/n) : ";
		string yn;
		cin >> yn;
		if (yn == "y" | yn == "Y")
		{
			cout << "Enter the name of a running process : ";
			cin >> proc;
			Process = SzToLPCTSTR(proc);
			DWORD id = MyGetProcessId(Process);
			if (id == 0)
			{
			}
			else
			{
				cout << "The process id is " << id << "......" << endl;
				cout << "Enter the Dll Path : ";
				cin >> szDllPath;
				cout << "Dll path is : " << szDllPath << endl;
				cout << "Injecting the dll ( " << szDllPath << ") into the running process ( " << proc << ")........." << endl;
				injectdll(id, szDllPath);
				cout << "--------------------------------------------------------------------------------" << endl;
			}
		}
		else if (yn == "n" | yn == "N")
		{
			return 0;
		}
	}
		system("pause");
		return 0;
}

