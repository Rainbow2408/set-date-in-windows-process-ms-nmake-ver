#include <windows.h>
#include "detours.h"
#include "setDate.h"

typedef void     (WINAPI  *GetSystemTime_func                 )(LPSYSTEMTIME  );
typedef void     (WINAPI  *GetLocalTime_func                  )(LPSYSTEMTIME  );

GetSystemTime_func GetSystemTime_real;
GetLocalTime_func  GetLocalTime_real ;

LPSYSTEMTIME       pFakeLocalTime = 0;
  SYSTEMTIME        fakeUniversalTime;

void WINAPI   GetSystemTime_hook (LPSYSTEMTIME t) {
   CopyMemory(t, &fakeUniversalTime, sizeof(SYSTEMTIME));
}

void WINAPI   GetLocalTime_hook  (LPSYSTEMTIME t) {
   CopyMemory(t, pFakeLocalTime, sizeof(SYSTEMTIME));
}


int attach(PVOID func, PVOID* real, PVOID hook) {

  *real = func;
  
   LONG dar;

   if ( (dar = DetourAttach(real, hook)) != NO_ERROR ) {
      return 0; 
   }

   return 1;
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
   (void)hModule;
   (void)lpReserved;

   switch ( dwReason ) {

      case DLL_PROCESS_ATTACH: {

         for (HMODULE hMod = NULL; (hMod = DetourEnumerateModules(hMod)) != NULL;) {
             ULONG cbData;
             PVOID pvData = DetourFindPayload(hMod, payload, &cbData);
       
             if (pvData != NULL) {
                 pFakeLocalTime = (LPSYSTEMTIME) pvData;

                 TzSpecificLocalTimeToSystemTime(0, pFakeLocalTime , &fakeUniversalTime);
       
              //
              // MSDN: Do not cast a pointer to a FILETIME structure to either a
              // ULARGE_INTEGER* or __int64* value because it can cause alignment
              // faults on 64-bit Windows.
              //
              // fakeLargeIntger.LowPart  = fakeFiletime.dwLowDateTime;
              // fakeLargeIntger.HighPart = fakeFiletime.dwHighDateTime;
       
             }
         }


         if (DetourTransactionBegin() != NO_ERROR) {
            return FALSE; 
         }

         if (  DetourUpdateThread( GetCurrentThread()) != NO_ERROR ) {
            return FALSE; 
         }

         HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
         GetSystemTime_func GetSystemTime_ = (GetSystemTime_func) GetProcAddress(kernelBase, "GetSystemTime");
         GetLocalTime_func  GetLocalTime_  = (GetLocalTime_func ) GetProcAddress(kernelBase, "GetLocalTime" );

         attach((PVOID) GetSystemTime_                , (PVOID*) &GetSystemTime_real                 , (PVOID) GetSystemTime_hook                 );
         attach((PVOID) GetLocalTime_                 , (PVOID*) &GetLocalTime_real                  , (PVOID) GetLocalTime_hook                  );
         if ( DetourTransactionCommit() != NO_ERROR) {
            return FALSE; 
         }
         break;
      }
      case DLL_PROCESS_DETACH: {

         DetourTransactionBegin();
         DetourUpdateThread( GetCurrentThread() );
         
         DetourTransactionCommit(); 
      }
      break;
   }

   return TRUE;
}
