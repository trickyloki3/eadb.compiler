/* dev.exe -p ..\db -o ..\build
 * to update the sqlite3 databases when 
 * the text database files changes make
 * life so much easier [trickyloki3] */

#include "util.h"
#include <windows.h>

#define is_fail(x) ((x) == INVALID_HANDLE_VALUE)
#define is_null(x) ((x) == NULL)
#define free_ptr(x) if(x) { free(x); (x) = NULL; }

int main(int argc, char * argv[]) {
    LPSTR lpPath = NULL;
    LPSTR lpFail = NULL;
    DWORD dwCount = 0;
    HANDLE hNotify = NULL;
    SYSTEMTIME scTime;
    STARTUPINFO scStub;
    PROCESS_INFORMATION scProcess;
    
    memset(&scStub, 0, sizeof(scStub));

    /* get directory to watch */
    lpPath = calloc(MAX_PATH, sizeof(char));
    if (parse_argv(argv, argc) ||
        is_null(lpPath) ||
        is_null(get_argv('p')) ||
        !GetFullPathName(get_argv('p'), MAX_PATH, lpPath, NULL) ) {
        fprintf(stderr, "Failed to resolved path %s: ", get_argv('p'));
        goto failed;
    }

    /* register directory notification */
    hNotify = FindFirstChangeNotification(lpPath, FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE);
    if (is_fail(hNotify) ||
        is_null(hNotify)) {
        fprintf(stderr, "Failed to get notifications for %s: ", lpPath);
        goto failed;
    }

    fprintf(stderr, "Watching %s directory.\n", lpPath);

    /* set the current directory to db.exe */
    if (is_null(get_argv('o')) ||
        !GetFullPathName(get_argv('o'), MAX_PATH, lpPath, NULL) ||
        !SetCurrentDirectory(lpPath)) {
        goto failed;
    }

    /* check directory notifications */
    while (1) {
        if (WAIT_OBJECT_0 == WaitForSingleObject(hNotify, INFINITE)) {
            /* limit the two notification per save to one update */
            if (dwCount % 2 == 0) {
                /* update the databases in the directory */
                if (!CreateProcess(NULL, "db.exe -p ..\\db -o . -m resource", NULL, NULL,
                    FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &scStub, &scProcess)) {
                    fprintf(stderr, "Failed to run db.exe: ");
                    goto failed;
                } else {
                    /* wait for process to complete */
                    WaitForSingleObject(scProcess.hProcess, INFINITE);
                    CloseHandle(scStub.hStdError);
                    CloseHandle(scStub.hStdInput);
                    CloseHandle(scStub.hStdOutput);
                    CloseHandle(scProcess.hProcess);
                    CloseHandle(scProcess.hThread);
                }

                GetSystemTime(&scTime);
                fprintf(stderr, "Updated database on %02u:%02u:%02u"
                ".\n", scTime.wHour, scTime.wMinute, scTime.wSecond);
            }

            /* register directory notification */
            if (!FindNextChangeNotification(hNotify))
                goto failed;

            dwCount++;
        }
    }
        
    FindCloseChangeNotification(hNotify);
    free_ptr(lpPath);
    return 0;

failed:
    /* write window error message */
    if (GetLastError()) {
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
            GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &lpFail, 0, 
            NULL)) {
            fprintf(stderr, "%s", lpFail);
            LocalFree(lpFail);
        }
    } else {
        fprintf(stderr, "try again.\n");
    }

    if (!is_null(hNotify) && 
        !is_fail(hNotify))
        FindCloseChangeNotification(hNotify);
    free_ptr(lpPath);
    return 1;
}
