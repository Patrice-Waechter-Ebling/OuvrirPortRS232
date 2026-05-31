// OuvrirPortRS232.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
#include <windows.h>
#include <stdio.h>
#pragma warning(disable:4996) // pour sprintf

static HANDLE OpenSerialPort(const char* portName, DWORD baud)
{
    char fullName[32];
    sprintf(fullName, "\\\\.\\%s", portName);
    HANDLE h = CreateFileA(fullName,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL );
    if (h == INVALID_HANDLE_VALUE) {
        printf("Erreur: impossible d'ouvrir %s\n", fullName);
        return INVALID_HANDLE_VALUE;
    }
    DCB dcb;
    ZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(h, &dcb)) {
        printf("Erreur GetCommState\n");
        CloseHandle(h);
        return INVALID_HANDLE_VALUE;
    }
    dcb.BaudRate = baud;          // 9600, 19200, 38400, 57600, 115200
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    dcb.fNull = FALSE;
    dcb.fAbortOnError = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fParity = FALSE;
    dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    // Ne pas activer DTR, certains appareils peuvent ne pas fonctionner correctement si DTR est activé
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    // Ne pas activer RTS, certains appareils peuvent ne pas fonctionner correctement si RTS est activé
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    if (!SetCommState(h, &dcb)) {
        printf("Erreur SetCommState\n");
        CloseHandle(h);
        return INVALID_HANDLE_VALUE;
    }
    COMMTIMEOUTS to;
    to.ReadIntervalTimeout = 50;
    to.ReadTotalTimeoutMultiplier = 10;
    to.ReadTotalTimeoutConstant = 50;
    to.WriteTotalTimeoutMultiplier = 10;
    to.WriteTotalTimeoutConstant = 50;
    SetCommTimeouts(h, &to);
    printf("Port %s ouvert a %lu kilooctets/s\n", portName, baud/1000);
    return h;
}
static int SerialRead(HANDLE h, char* buffer, DWORD maxLen)
{
    DWORD bytesRead = 0;
    if (!ReadFile(h, buffer, maxLen, &bytesRead, NULL))        return -1;
    buffer[bytesRead] = 0; // null-terminate
	printf("Reception:%s (%lu octets)\n", buffer, bytesRead);
    return bytesRead;
}
static bool SerialWrite(HANDLE h, const char* data)
{
    DWORD written = 0;
	printf("Envoi: %s\n", data);
    return WriteFile(h, data, strlen(data), &written, NULL);
}
int main()
{
    HANDLE h = OpenSerialPort("COM1", 115200);
    if (h == INVALID_HANDLE_VALUE) return 1;
    SerialWrite(h, "show version\r");
    char buf[512];
    int n = SerialRead(h, buf, sizeof(buf) - 1);
    if (n > 0)
        printf("Réponse:\n%s\n", buf);
    CloseHandle(h);
    return 0;
}
