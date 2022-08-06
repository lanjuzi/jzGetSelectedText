#include "jzGetSelectedText.h"
#include <stdio.h>

void handle_selected_text(LPSTR text) {
    printf("Selected text: %s\n", text);
}

int main() {
    const int MSG_ID1 = 0x2345;
    const int MSG_ID2 = MSG_ID1 + 1;
    MSG msg = {0};
    HWND hwnd = GetActiveWindow();

    RegisterHotKey(hwnd, MSG_ID1, MOD_CONTROL, 'D');// CTRL + D
    RegisterHotKey(hwnd, MSG_ID2, MOD_CONTROL, 'F');// CTRL + F

    while (GetMessage(&msg, hwnd, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == MSG_ID1) {
                LPTSTR text;
                if (jz_get_selected_text(&text, NULL)) {
                    printf("Selected text: %s\n", text);
                    free(text);
                } else {
                    printf("Didn't get to the text\n");
                }
            } else if (msg.wParam == MSG_ID2) {
                if (!jz_get_selected_text_callback(handle_selected_text)) {
                    printf("Didn't get to the text\n");
                }
            }
        }
    }

    return 0;
}