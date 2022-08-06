#ifndef JZGETSELECTEDTEXT_JZGETSELECTEDTEXT_H
#define JZGETSELECTEDTEXT_JZGETSELECTEDTEXT_H

#include <windows.h>

typedef void (*jz_callback)(LPTSTR out);

BOOL jz_get_selected_text(LPTSTR *text, size_t *size);

BOOL jz_get_selected_text_callback(jz_callback cb);

BOOL jz_open_clipboard(HWND hWnd);

UINT jz_send_input(WORD wVk, DWORD dwFlags);

#endif//JZGETSELECTEDTEXT_JZGETSELECTEDTEXT_H
