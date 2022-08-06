#include "jzGetSelectedText.h"

#include <stdio.h>


BOOL jz_open_clipboard(HWND hWnd) {
    int retries = 0;
    while (TRUE) {
        if (retries) {
            Sleep(5u);
        }
        if (OpenClipboard(hWnd)) {
            break;
        }
        if (++retries >= 5) {
            return FALSE;
        }
    }
    return TRUE;
}

UINT jz_send_input(WORD wVk, DWORD dwFlags) {
    INPUT pInputs[1];
    memset(&pInputs, 0, sizeof(pInputs));
    pInputs[0].type = INPUT_KEYBOARD;
    pInputs[0].ki.wVk = wVk;
    pInputs[0].ki.dwFlags = dwFlags;
    return SendInput(1, pInputs, sizeof(pInputs));
}


BOOL jz_get_selected_text(LPTSTR *text, size_t *size) {
    BOOL result = FALSE;
    HWND hWnd = NULL;

    if (!jz_open_clipboard(hWnd)) {
        return result;
    }

    UINT clipboardFormat = 0;
    HGLOBAL hMemOrigin;
    LPCTSTR lpStrOrigin;
    size_t lpStrOriginSize;

    clipboardFormat = EnumClipboardFormats(clipboardFormat);
    while (clipboardFormat) {
        if (clipboardFormat != CF_BITMAP && clipboardFormat != CF_METAFILEPICT && clipboardFormat != CF_PALETTE && clipboardFormat != CF_ENHMETAFILE && clipboardFormat != CF_OWNERDISPLAY && clipboardFormat != CF_DSPENHMETAFILE && clipboardFormat != CF_DSPBITMAP && clipboardFormat != CF_DSPMETAFILEPICT && (clipboardFormat < CF_PRIVATEFIRST || clipboardFormat > CF_PRIVATELAST)) {
            hMemOrigin = GetClipboardData(clipboardFormat);
            break;
        }

        clipboardFormat = EnumClipboardFormats(clipboardFormat);
    }

    if (hMemOrigin != NULL) {
        lpStrOrigin = (LPCTSTR) GlobalLock(hMemOrigin);
        lpStrOriginSize = GlobalSize(hMemOrigin);
        GlobalUnlock(hMemOrigin);
    }

    CloseClipboard();

    while ((UINT16) GetKeyState(VK_MENU) >> 8 || (UINT16) GetKeyState(VK_SHIFT) >> 8) {
        Sleep(5);
    }

    UINT clipboardSequenceNumber = GetClipboardSequenceNumber();

    // CTRL + C => DOWN
    if (!jz_send_input(VK_CONTROL, 0) || !jz_send_input(0x43, 0)) {
        return result;
    }
    // CTRL + C => UP
    if (!jz_send_input(VK_CONTROL, KEYEVENTF_KEYUP) || !jz_send_input(0x43, KEYEVENTF_KEYUP)) {
        return result;
    }

    Sleep(4);


    int delay = 8;
    while (GetClipboardSequenceNumber() == clipboardSequenceNumber) {
        if (delay >= 320) {
            return result;
        }
        Sleep(delay);
        delay *= 2;
    }
    Sleep(4);

    if (!jz_open_clipboard(hWnd)) {
        return result;
    }

    UINT uiFormat = (sizeof(TCHAR) == sizeof(WCHAR)) ? CF_UNICODETEXT : CF_TEXT;
    HGLOBAL hMemSelected = GetClipboardData(uiFormat);
    if (hMemSelected != NULL) {
        LPTSTR lpStr = (LPTSTR) GlobalLock(hMemSelected);
        size_t lpSize = GlobalSize(hMemSelected);
        if ((*text = malloc(lpSize)) != NULL) {
            memcpy(*text, lpStr, lpSize);
            if (size != NULL) {
                *size = lpSize;
            }
            result = TRUE;
        }
        GlobalUnlock(hMemSelected);
    }

    CloseClipboard();

    if (hMemOrigin != NULL) {
        if (!jz_open_clipboard(hWnd)) {
            return result;
        }

        EmptyClipboard();
        HGLOBAL hMemNew = GlobalAlloc(GMEM_MOVEABLE, lpStrOriginSize);
        if (hMemNew == NULL) {
            CloseClipboard();
            return result;
        }
        memcpy(GlobalLock(hMemNew), lpStrOrigin, lpStrOriginSize);
        GlobalUnlock(hMemNew);

        SetClipboardData(clipboardFormat, hMemNew);

        CloseClipboard();
    }

    return result;
}

BOOL jz_get_selected_text_callback(jz_callback callback) {
    LPTSTR text;
    size_t size;
    if (jz_get_selected_text(&text, &size)) {
        callback(text);
        free(text);
        return TRUE;
    }

    return FALSE;
}
