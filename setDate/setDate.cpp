#include <windows.h>
#include <iostream>
#include <vector>
#include "detours.h"
#include "setDate.h"

bool containsUnicode(const wchar_t *str)
{
   while (*str)
   {
      if (*str > 127)
      {
         return true;
      }
      ++str;
   }
   return false;
}

int wmain(int argc, wchar_t *argv[]) {

   if (argc < 2) {
      std::wcout << L"specify executable\n";
      return 1;
   }

   STARTUPINFOW        si; ZeroMemory(&si, sizeof(si));
   PROCESS_INFORMATION pi; ZeroMemory(&pi, sizeof(pi));

   si.cb = sizeof(si);
   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_SHOW;

   std::vector<wchar_t> w_dllPath(MAX_PATH);
   DWORD pathLength = GetModuleFileNameW(NULL, w_dllPath.data(), MAX_PATH);
   if (pathLength == 0)
   {
      std::cerr << "Failed to get module file name." << std::endl;
      return 1;
   }
   wchar_t *cp = w_dllPath.data() + pathLength;
   while (*--cp != L'\\');
   const wchar_t *dllName = L"setDate.dll";
   wmemcpy(cp + 1, dllName, wcslen(dllName) + 1);

   // Convert Path to ANSI for dll use.
   if (containsUnicode(w_dllPath.data())){
      std::wcerr << L"Unicode path not supported." << std::endl;
      return 1;
   }
   int size_needed = WideCharToMultiByte(CP_ACP, 0, w_dllPath.data(), -1, nullptr, 0, nullptr, nullptr);
   if (size_needed == 0)
   {
      throw std::runtime_error("WideCharToMultiByte failed.");
   }
   std::vector<char> dllPath(size_needed);
   WideCharToMultiByte(CP_ACP, 0, w_dllPath.data(), -1, dllPath.data(), size_needed, nullptr, nullptr);

   wchar_t *lpCommandLine = GetCommandLineW();

   lpCommandLine += wcslen(argv[0]) + 2; // Skip program name plus 2 apostrophes in which it is enclosed.
   std::wcout << lpCommandLine << L"\n";

   while (*lpCommandLine == L' ') *++lpCommandLine;

   SYSTEMTIME fake_SYSTEMTIME;

   *(lpCommandLine + 4) = 0; fake_SYSTEMTIME.wYear         = (WORD)_wtoi(lpCommandLine); lpCommandLine += 5;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wMonth        = (WORD)_wtoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wDay          = (WORD)_wtoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wHour         = (WORD)_wtoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wMinute       = (WORD)_wtoi(lpCommandLine); lpCommandLine += 3;
   *(lpCommandLine + 2) = 0; fake_SYSTEMTIME.wSecond       = (WORD)_wtoi(lpCommandLine); lpCommandLine += 3;
                             fake_SYSTEMTIME.wDayOfWeek    = 0;  // Hah. How am I supposed to know?
                             fake_SYSTEMTIME.wMilliseconds = 0;  // Granularity of one second is enough for everyone...

   while (*lpCommandLine == L' ') lpCommandLine++;

   if (!DetourCreateProcessWithDllExW(
       0                           , // LPCTSTR                         appName
       lpCommandLine               , // LPTSTR                          lpCommandLine,
       0                           , // LPSECURITY_ATTRIBUTES           lpProcessAttributes,
       0                           , // LPSECURITY_ATTRIBUTES           lpThreadAttributes,
       0                           , // BOOL                            bInheritHandles,
       CREATE_DEFAULT_ERROR_MODE |
       CREATE_SUSPENDED            , // DWORD                           dwCreationFlags,
       0                           , // LPVOID                          lpEnvironment,
       0                           , // LPCTSTR                         lpCurrentDirectory,
      &si                          , // LPSTARTUPINFOW                  lpStartupInfo,
      &pi                          , // LPPROCESS_INFORMATION           lpProcessInformation,
       dllPath.data()              , // LPCSTR                          lpDllName
       0                             // PDETOUR_CREATE_PROCESS_ROUTINEW pfCreateProcessW
   )) {
      std::wcerr << L"Failed to create process with DLL." << std::endl;
      return -1;
   }

   // https://github.com/microsoft/Detours/blob/master/samples/tracebld/tracebld.cpp
   if (!DetourCopyPayloadToProcess(
      pi.hProcess,
      payload,
      &fake_SYSTEMTIME,
      sizeof(SYSTEMTIME))
   ) {
      std::wcout << "failed copy payload" << std::endl;
      return 1;
   }


   ResumeThread(pi.hThread);

   WaitForSingleObject(pi.hProcess, INFINITE);

   CloseHandle(&si);
   CloseHandle(&pi);

   return 0;
}
