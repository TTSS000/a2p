// a2p64.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "a2p64.h"
#include <commdlg.h>
#include <shellapi.h>


#define MAX_LOADSTRING 100
#define APPNAME L"a2p"
#define MY_INIT 0
#define FOUND_FIRST_SPACE 1
#define FOUND_OPTION 2
#define FOUND_NEXT_SPACE 3
#define FOUND_FILENAME 4
#define OPTION_ERR 5
#define DONE 6
#define FOUND_SPACE 7
#define IN_DBL_QUOTE 8
#define INI_FILE_ENTRY L"a2p"
#define INI_FILE_NAME L"a2p.ini"
#define MYEXTENSION L"*.sj*"
#define PWDLEN 64
#define MYCHARBUFLEN 256
#define APP_HELP_TEXT L"\\a2p.txt"
#define APP_HELP_DOC L"\\a2p.doc"
#define APP_HELP_FILE L"\\a2p.hlp"
#define THUMB_MAX 511
#define THUMB_STEP 1
#define DIGIT 5
#define DOTS_FROM_EDGE 5
#define IDM_PRINT 4000
#define IDM_Quit                        4001
#define IDM_Viewer                      4002
#define IDM_Help                        4003
#define IDM_About                       4004
#define IDM_JisRead                     4005
#define IDM_Jis2Sjis                    4006
#define IDM_Sjis2Jis                    4007
#define IDM_Uuencode                    4008
#define IDM_Uudecode                    4009
#define IDM_Probe                       4010
#define IDM_Split                       4011
#define IDM_BatchViewer                 4012
#define IDM_BatchJis2Sjis               4013
#define IDM_BatchSjis2Jis               4014
#define IDM_ContentLength               4015
#define IDM_Read                        4016
#define IDM_Base64decode                4017
#define IDM_FileMask_independent        4018
#define IDM_ChangeDiskDirectory         4019
#define IDM_UpdateList                  4020
#define IDM_Base64encode_text           4021
#define IDM_AddMimeVersion              4022
#define IDM_Dump                        4023
#define IDM_KIN_KOUT                    4024
#define IDM_FONT                        4025
#define IDM_Length                      4026
#define IDM_Base64decodeBinary          4027
#define IDM_HelpText                    4028
#define IDM_Left_Upper                  4029
#define IDM_OPEN_PRINT 4030
#define IDM_Search 4031
#define IDM_Search_Jump 4032
#define IDM_Search_RMAIL_Jump 4033
#define IDM_SearchFromList 4034
#define IDM_Return_hWndList 4035
#define IDM_Close 4036
#define IDM_FONT_SIZE_ADJUST 4037
#define IDM_LOADFILE 4038
#define IDM_DO1 4039
#define IDM_DO2 4040
#define IDM_CUT 4041
#define IDM_COPY 4042
#define IDM_PASTE 4043
#define IDM_SELECTALL 4044
#define IDM_HelpDoc 4045
#define IDM_DO_I_EXIST 4046
#define IDM_UPDATE_TRANS 4047
#define IDM_RECEIVE_PARAM_FROM_WINMAIN 4048
#define IDM_COPY_LOGFONT 4049

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HWND _hDlgPrintAbort = NULL; /* handle of print abort dlg box */
BOOL _bPrintAbort = false; /* bool is TRUE to abort printing */

/*----------------------------------------------------------------------*/
// this is the dialog box function for the dialog box that is
// shown on the screen while the program is printing
BOOL WINAPI PrintStopDlg(HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    if (_hDlgPrintAbort == NULL) {
        _hDlgPrintAbort = hDlg;
    }
    if (wMessage == WM_COMMAND) {
        /* any WM_COMMAND comes from it
           there is only one button, so
        */
        _bPrintAbort = TRUE; // changes the global variable
        //DestroyWindow(hDlg);
        return TRUE; // _bPrintAbort to TRUE
    }
    else {
        return FALSE;
    }
}
/*----------------------------------------------------------------------*/
// this is the message processing function that gets messages
// while the printer abort dialog box is on the screen
BOOL WINAPI PrintAbort(HDC hdcPrinter, int nCode)
{
    MSG msg;
    /* the global variable _bPrintAbort is set to TRUE
       if the user clicks the button on the abort dialog
       box.  This exits this message loop, returning
       control back to the program's message loop
    */

    while (!_bPrintAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (!IsDialogMessage(_hDlgPrintAbort, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (!_bPrintAbort);
}
/*----------------------------------------------------------------------*/
BOOL getfilename(WCHAR* command_line, int* pi, int* pstate,
    WCHAR* filename)
{
    filename[0] = '\0';
    int j = 0;
    int found_dbl_quote = 0;

    /* Think about the following case
      "C:\C_Program\rmailrd.008\t e s t\rmailrd.EXE" -r -o
      "C:\C_Program\rmail.011\t e s t\rmail32.txt"
    */

    while (command_line[*pi] != NULL
        && ((command_line[*pi] != ' ' && found_dbl_quote == 0)
            || found_dbl_quote == 1)
        && j < MAX_PATH) {
        if (command_line[*pi] == '"') {
            found_dbl_quote = (found_dbl_quote == 0) ? 1 : 0;
        }
        filename[j] = command_line[*pi];
        j++;
        (*pi)++;
    }
    if (found_dbl_quote == 1) {
        /* dbl quote should be a pair */
        *pstate = OPTION_ERR;
        return FALSE;
    }
    if (j >= MAX_PATH) {
        *pstate = OPTION_ERR;
        return FALSE;
    }
    else {
        filename[j] = NULL;
    }
    if (command_line[*pi] == NULL) {
        *pstate = DONE;
    }
    else if (command_line[*pi] == ' ') {
        *pstate = FOUND_SPACE;
    }
#if 0
    WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
        (LPSTR)"filename", (LPSTR)filename, (LPSTR)INI_FILE_NAME);
#endif
    return TRUE;
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/** Parse command line options */
/* Find the first space.
 Is the character after space characters '-' or '/'?
 If yes, read until the next space is found.
 If no, that is a file name. And only one file name is valid.
 example : a2p -q filename
           "a2p" "-q" "filename"
 If the filename is null, then don't read any file */
 /* when going to the next state,
  it always should point out the next letter */
BOOL ParseCommandLine(WCHAR* command_line, WCHAR* filename,
    BOOL* pbIsQuitMode)
{
    int i = 0, j = 0;
    int state = MY_INIT;
    int r = 0, x = 0, y = 0, p = 0, o = 0, f = 0;
    int found_first_set_of_dbl_quote = 0;
    int found_dbl_quote_b = 0;
    int parsed_my_self = 0;
    WCHAR myself[MYCHARBUFLEN] = { 0 };

    ZeroMemory(myself, sizeof(WCHAR) * MYCHARBUFLEN);
#if 0
    char tmpchar[256];
#endif
    HKEY hkResult;
    DWORD dwDisposition;

    while (command_line[i] != NULL
        && state != OPTION_ERR
        && state != DONE) {
        switch (command_line[i]) {
        case '"':
            if (0 == found_first_set_of_dbl_quote
                && 0 == parsed_my_self) {
                i++;
                /* skip the first pair of the dbl quotation */
                while (command_line[i] != '"'
                    && command_line[i] != NULL) {
                    myself[j] = command_line[i];
                    i++; j++;
                }
                myself[j] = '\0';
#if 0
                MessageBox(GetDesktopWindow(), myself, "DEBUG", MB_OK);
#endif
                if (command_line[i] == NULL) {
                    /* can't find the second dbl quotaton */
                    state = OPTION_ERR;
                }
                else {
                    found_first_set_of_dbl_quote = 1;
                    parsed_my_self = 1;
                    i++;
                }
            }
            else if (command_line[i + 1] == '-' || command_line[i + 1] == '/') {
                state = FOUND_OPTION;
                found_dbl_quote_b = 1;
                i++; i++;
            }
            else {
                if (found_dbl_quote_b == 0) {
                    state = FOUND_FILENAME;
                    /* read until space */
                    f = 1;
                    getfilename(command_line, &i, &state, filename);
                    /* Think about the following case
                      "C:\C_Program\rmailrd.008\t e s t\rmailrd.EXE" -r -o
                      "C:\C_Program\rmail.011\t e s t\rmail32.txt"
                    */
                }
                else {
                    found_dbl_quote_b = 0;
                    i++;
                }
            }
            break;
        case ' ':
            while (command_line[i] == ' '
                && command_line[i] != NULL) {
                i++;
            }
#if 0
            if (state == MY_INIT) {
                state = FOUND_FIRST_SPACE;
            }
            else {
                state = FOUND_NEXT_SPACE;
            }
#endif
            if (found_first_set_of_dbl_quote == 0) {
                parsed_my_self = 1;
            }
            state = FOUND_SPACE;
            break;
        case '-':
        case '/':
            state = FOUND_OPTION;
            i++;
            break;
        default:
            switch (state) {
            case FOUND_SPACE:
                /* This means the previous letter is space,
                   and the current one is neither space nor '-','/' */
                state = FOUND_FILENAME;
                /* read until space */
                f = 1;
                getfilename(command_line, &i, &state, filename);
                /* Think about the following case
                  "C:\C_Program\rmailrd.008\t e s t\rmailrd.EXE" -r -o
                  "C:\C_Program\rmail.011\t e s t\rmail32.txt"
                */
                break;
            case FOUND_OPTION:
                switch (command_line[i]) {
#if 0
                case 'r':
                case 'R':
                    r = 1;
                    i++;
                    break;
                case 'o':
                case 'O':
                    o = 1;
                    i++;
                    break;
                case 'x':
                case 'X':
                    /* need,
                       command_line, i, state, x */
                       /* Shall I make function and structure? */
                    i++;
                    getnumber(command_line, &i, &state, &x);
                    break;
                case 'y':
                case 'Y':
                    /* skip space */
                    /* read next number */
                    i++;
                    getnumber(command_line, &i, &state, &y);
                    break;
                case 'p':
                case 'P':
                    /* get parent window */
                    /* skip space */
                    /* read next number */
                    i++;
                    getnumber(command_line, &i, &state, &p);
                    break;
#endif
                case 'q':
                case 'Q':
                    *pbIsQuitMode = TRUE;
                    i++;
                    break;
                default:
                    state = OPTION_ERR;
                    i++;
                    break;
                }
                break;
            default:
                i++;
                break;
            }
            //      i++
            break;
        }
    }

    if (state != OPTION_ERR) {
        RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\WhiteRock\\a2p",
            NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
            &hkResult, &dwDisposition);
        RegSetValueEx(hkResult, L"myself", NULL, REG_SZ,
            (CONST BYTE*) myself, lstrlen(myself) + 1);
        RegCloseKey(hkResult);
#if 0
        wsprintf(tmpchar, "%ld", r);
        WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
            (LPSTR)"READONLY", (LPSTR)tmpchar, (LPSTR)INI_FILE_NAME);
        wsprintf(tmpchar, "%ld", o);
        WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
            (LPSTR)"ONEWINDOW", (LPSTR)tmpchar, (LPSTR)INI_FILE_NAME);
        wsprintf(tmpchar, "%ld", p);
        WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
            (LPSTR)"hWndParent", (LPSTR)tmpchar, (LPSTR)INI_FILE_NAME);
        wsprintf(tmpchar, "%ld", x);
        WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
            (LPSTR)"X", (LPSTR)tmpchar, (LPSTR)INI_FILE_NAME);
        wsprintf(tmpchar, "%ld", y);
        WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
            (LPSTR)"Y", (LPSTR)tmpchar, (LPSTR)INI_FILE_NAME);


        if (f == 0) {
            WritePrivateProfileString((LPSTR)INI_FILE_ENTRY,
                (LPSTR)"filename", (LPSTR)NULL, (LPSTR)INI_FILE_NAME);
        }
#endif
        return TRUE;
    }
    return FALSE;
}

#if 0
void getnumber(char* command_line, int* pi, int* pstate, int* ppara)
{
    int j;
    char tmpchar[256];

    /* skip space */
    while (command_line[*pi] == ' '
        && command_line[*pi] != NULL) {
        (*pi)++;
    }
    /* the next letter must a figure */
    if (command_line[*pi] == NULL
        || command_line[*pi] < '0'
        || command_line[*pi] > '9') {
        *pstate = OPTION_ERR;
    }
    /* read next number */
    if (*pstate != OPTION_ERR) {
        j = 0;
        while (command_line[*pi] != NULL
            && command_line[*pi] >= '0'
            && command_line[*pi] <= '9') {
            tmpchar[j] = command_line[*pi];
            (*pi)++;
            if (j < 255) {
                j++;
            }
            else {
                /* the number is too large. */
                *pstate = OPTION_ERR;
            }
        }
        tmpchar[j] = NULL;
        if (*pstate != OPTION_ERR) {
            *ppara = strtol(tmpchar, NULL, 10);
            if (command_line[*pi] == NULL) {
                *pstate = DONE;
            }
        }
    }
}
#endif
/*----------------------------------------------------------------------*/

DWORD g_dwX = 0, g_dwY = 0, g_dwWidth = 160, g_dwHeight = 100;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WNDCLASS wndclass;
    HWND hWnd;
    //MSG msg;
    static WCHAR szAppName[] = APPNAME;
    static WCHAR stdmsg[256], command_line[256], filename[256];
    static WCHAR wtmpbuf[256];
    static unsigned char tmpbuf[256];
    HKEY hkResult;
    DWORD dwDisposition = 0, dwType = 0;
    DWORD cbData = 0, dwKeyValue = 0;
    BOOL bIsQuitMode = FALSE;
    long lError = 0;

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_A2P64, szWindowClass, MAX_LOADSTRING);

    lstrcpy(command_line, L"\"dummy\" ");
    lstrcat(command_line, lpCmdLine);
    ParseCommandLine(command_line, filename, &bIsQuitMode);

    if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
        L"Software\\WhiteRock\\a2p", 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_EXECUTE,
        NULL, &hkResult, &dwDisposition)) {
    }

    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"hWnd", NULL,
        &dwType, (LPBYTE)tmpbuf, &cbData)) {
        ULONG64 temphwnd = NULL;
        temphwnd = tmpbuf[7];
        temphwnd = temphwnd << 8;
        temphwnd = temphwnd | tmpbuf[6];
        temphwnd = temphwnd << 8;
        temphwnd = temphwnd | tmpbuf[5];
        temphwnd = temphwnd << 8;
        temphwnd = temphwnd | tmpbuf[4];
        temphwnd = temphwnd << 8;
        temphwnd = temphwnd | tmpbuf[3];
        temphwnd = temphwnd << 8;
        temphwnd = temphwnd | tmpbuf[2];
        temphwnd = temphwnd << 8;
        temphwnd = temphwnd | tmpbuf[1];
        temphwnd = temphwnd << 8;
        hWnd = (HWND)(temphwnd | tmpbuf[0]);




    }
    else {
        hWnd = 0;
    }
    RegCloseKey(hkResult);

    /* check the window exists or not */
    if (hWnd != 0 && bIsQuitMode == FALSE) {
        GetClassName(hWnd, wtmpbuf, 256);
        if (0 != GetWindowTextLength(hWnd)
            && !lstrcmp(wtmpbuf, szAppName)) {
            /* if exists, then set it to forground */
            SetForegroundWindow(hWnd);
            if (lstrcmp(lpCmdLine, L"")) {
                /* must path filename */
                PostMessage(hWnd, WM_COMMAND,
                    MAKELONG(IDM_PRINT, 0), (LPARAM)lpCmdLine);
            }
            return FALSE;
        }
    }

    //wsprintf(stdmsg, L"hPrevInstance = %x",
    //    hPrevInstance);
    //MessageBox(GetDesktopWindow(), stdmsg, L"DEBUG", MB_OK);

    if (!hPrevInstance) {
        /* main window's class */
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = WndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(hInstance, L"A2P_ICON");
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        //    wndclass.hbrBackground = GetStockObject(GRAY_BRUSH);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        //    wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = L"MYIDR_A2P";
        //wndclass.lpszClassName = szAppName;
        wndclass.lpszClassName = szWindowClass;


        RegisterClass(&wndclass);
    }

    //MyRegisterClass(hInstance);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_A2P64));



    if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
        L"Software\\WhiteRock\\a2p", 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_EXECUTE,
        NULL, &hkResult, &dwDisposition)) {
#if 0
        MessageBox(GetDesktopWindow(), L"reg create OK", L"DEBUG", MB_OK);
#endif
    }

    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"X", NULL,
        &dwType, (LPBYTE)NULL, &cbData)) {
#if 0
        MessageBox(GetDesktopWindow(), L"reg query0 OK", L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"X", NULL,
        &dwType, (LPBYTE)tmpbuf, &cbData)) {
        g_dwX = tmpbuf[0] + (tmpbuf[1] << 8)
            + (tmpbuf[2] << 16) + (tmpbuf[3] << 24);
#if 0
        wsprintf(stdmsg, L"reg query2 OK dwX = %x %x %x %x %x",
            g_dwX, tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3]);
        MessageBox(GetDesktopWindow(), stdmsg, L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"Y", NULL,
        &dwType, (LPBYTE)NULL, &cbData)) {
#if 0
        MessageBox(GetDesktopWindow(), L"reg query0 OK", L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"Y", NULL,
        &dwType, (LPBYTE)tmpbuf, &cbData)) {
        g_dwY = tmpbuf[0] + (tmpbuf[1] << 8)
            + (tmpbuf[2] << 16) + (tmpbuf[3] << 24);
#if 0
        wsprintf(stdmsg, L"reg query3 OK tmpbuf = %x %x %x %x",
            tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3]);
        MessageBox(GetDesktopWindow(), stdmsg, L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"nWidth", NULL,
        &dwType, (LPBYTE)NULL, &cbData)) {
#if 0
        MessageBox(GetDesktopWindow(), L"reg query0 OK", L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"nWidth", NULL,
        &dwType, (LPBYTE)tmpbuf, &cbData)) {
        g_dwWidth = tmpbuf[0] + (tmpbuf[1] << 8)
            + (tmpbuf[2] << 16) + (tmpbuf[3] << 24);
#if 0
        wsprintf(stdmsg, L"reg query4 OK tmpbuf = %x %x %x %x",
            tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3]);
        MessageBox(GetDesktopWindow(), stdmsg, L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"nHeight", NULL,
        &dwType, (LPBYTE)NULL, &cbData)) {
#if 0
        MessageBox(GetDesktopWindow(), L"reg query0 OK", L"DEBUG", MB_OK);
#endif
    }
    if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"nHeight", NULL,
        &dwType, (LPBYTE)tmpbuf, &cbData)) {
        g_dwHeight = tmpbuf[0] + (tmpbuf[1] << 8)
            + (tmpbuf[2] << 16) + (tmpbuf[3] << 24);
#if 0
        wsprintf(stdmsg, L"reg query5 OK tmpbuf = %x %x %x %x",
            tmpbuf[0], tmpbuf[1], tmpbuf[2], tmpbuf[3]);
        MessageBox(GetDesktopWindow(), stdmsg, L"DEBUG", MB_OK);
#endif
    }

    RegCloseKey(hkResult);

#if 0
    g_dwX = 0; g_dwY = 0;
    wsprintf(stdmsg, L"xywh cbData lError hkResult = %d, %d, %d, %d, %d, %d %d",
        g_dwX, g_dwY, g_dwWidth, g_dwHeight, cbData, lError, hkResult);
    MessageBox(hWnd, stdmsg, L"DEBUG", MB_OK);
#endif

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }
    /* append item to the system menu */
    //MessageBox(GetDesktopWindow(), L"after init", L"DEBUG", MB_OK);

    if (bIsQuitMode == FALSE) {
        RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\WhiteRock\\a2p",
            NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
            &hkResult, &dwDisposition);
        //dwKeyValue = (QWORD)hWnd;
        RegSetValueEx(hkResult, L"hWnd", NULL, REG_QWORD,
            (CONST BYTE*) & hWnd, sizeof(hWnd));
        RegCloseKey(hkResult);
    }

    //MessageBox(GetDesktopWindow(), L"after init 001", L"DEBUG", MB_OK);
    //ShowWindow(hWnd, nCmdShow);
    //UpdateWindow(hWnd);

    MSG msg;

    // メイン メッセージ ループ:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;



    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_A2P64));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_A2P64);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   //MessageBox(GetDesktopWindow(), L"before CreateWindowEx 001", L"DEBUG", MB_OK);

   HWND hWnd = CreateWindowEx(WS_EX_ACCEPTFILES,
       szWindowClass,
       APPNAME,
       WS_OVERLAPPEDWINDOW /* WS_OVERLAPPED
       | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX */,

       g_dwX, g_dwY, g_dwWidth, g_dwHeight,
       /*
             CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
       */
       NULL, NULL, hInstance, NULL);

   //MessageBox(GetDesktopWindow(), L"after CreateWindowEx 001", L"DEBUG", MB_OK);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
/*----------------------------------------------------------------------*/


/* These values are based on 0.1mm */
/* must be modified to 0.01mm */
#define LINE_LEFT1 2100
#define LINE_LEFT2 15500
#define LINE_RIGHT1 14490
/*
#define LINE_RIGHT2 27740
*/
#define LINE_RIGHT2 27890
#define LINE_UPPER -1910
#define LINE_LOWER -19800
#define LINE_TITLE -2500
/* difference from actual printer */
#define DIFFX -520
#define DIFFY 380
#define PAGEY -2350
#define START_PTY -2950
#define END_PTY -19680
/* line start point from left */
#define START_PTX1 2400
#define START_PTX2 15750
#define TITLEY -2350
#define KANJIASCIIDIFFY -50
#define DATEX1 2420
#define PAGEX1 13040
#define TITLEX1 8900
#define DATEX2 15750
#define PAGEX2 26340
#define TITLEX2 22250
#define ORIKAESHI1 14200
#define ORIKAESHI2 27550
#define FOOTTITLEX 2100
#define FOOTTITLEY -20150
#define FOOTPAGEX 27550
/*----------------------------------------------------------------------*/
typedef struct {
    HDC hDCPrinter;
    WCHAR text[256];
    WCHAR c[8];
    WCHAR precroman[256];
    WCHAR croman[256];
    int line;
    int kanjimode;
    HFONT hFontMainRoman;
    HFONT hFontMainKanji;
    POINT start_pt;
    POINT end_pt;
    POINT rectangle_pt;
} BF_W;
/*----------------------------------------------------------------------*/
typedef struct {
  HDC hDCPrinter;
  char text[256];
  char c[8];
  char precroman[256];
  char croman[256];
  int line;
  int kanjimode;
  HFONT hFontMainRoman;
  HFONT hFontMainKanji;
  POINT start_pt;
  POINT end_pt;
  POINT rectangle_pt;
} BF;
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* is this file hankaku? */
int ishankakuA(char c)
{
    if (c >= 0xa0 && c <= 0xdf) {
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------*/
int ishankakuW(WCHAR c)
{
  if (c >= 0xa0 && c <= 0xdf) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_sjis_1stA(char c)
{
    if (c >= 0x81 && c <= 0x9f) {
        return 1;
    }
    if (c >= 0xe0 && c <= 0xef) {
        return 1;
    }
    return 0;
}
/*----------------------------------------------------------------------*/
int is_sjis_1stW(WCHAR c)
{
  static int count = 0;

  if (c >= 0x81 && c <= 0x9f) {
   // if (count < 10) {
      //MessageBox(NULL, L"sjis 1st hit", L"DEBUG", MB_OK);
    //}
    count++;
    return 1;
  }
  if (c >= 0xe0 && c <= 0xef) {
 //   if (count < 10) MessageBox(NULL, L"sjis 1st hit", L"DEBUG", MB_OK);
    count++;
      return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_lead_2byteZenW(WCHAR c)
{
  // 1100 0000
  if (0xC0 <= c && c < 0xE0) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_lead_3byteZenW(WCHAR c)
{
  if (0xE0 <= c && c < 0xF0) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_lead_4byteZenW(WCHAR c)
{
  if (0xF0 <= c && c < 0xF8) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_2byte_hankakuW(WCHAR c0, WCHAR c1)
{
  // 80 - ff
  //0x80 = 110 00010    10 000000
  //0xFF = 110 00011    10 111111
  //110 xxxxx    10 xxxxxx

  unsigned long c_all = c0 * 256 + c1;

  if (0xC280 <= c_all && c_all <= 0xC3BF) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_3byte_hankakuW(WCHAR c0, WCHAR c1, WCHAR c2)
{

  unsigned long c_all = c0 * 256*256 + c1*256 + c2;

  if (0xEFBDA1 <= c_all && c_all <= 0xEFBE9F) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_hankaku_kanaW(WCHAR c0, WCHAR c1, WCHAR c2, WCHAR c3)
{
  if (is_utf8_2byte_hankakuW(c0, c1)) {
    return 1;
  }
  else   if (is_utf8_3byte_hankakuW(c0, c1, c2)) {
    return 1;
  }
  return 0;
}
/*----------------------------------------------------------------------*/
int is_utf8_zenkakuW(WCHAR c0, WCHAR c1, WCHAR c2, WCHAR c3)
{
  if (c0 <= 0x7f) return 0;
  if (is_utf8_hankaku_kanaW(c0, c1, c2, c3)) return 0;
  return 1;
}
/*----------------------------------------------------------------------*/
void buffer_flushA(HWND hWnd, HDC hDCPrinter, BF* bf)
{
    int i;
    int kanjiasciidiffy;
    static SIZE size;
    static int rkdiffdiv2 = 0;

    SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
    SetTextAlign(hDCPrinter, TA_BASELINE);
    /* zenkaku must stay center of two hankaku */
    if (rkdiffdiv2 == 0) {
        SelectObject(hDCPrinter, bf->hFontMainRoman);
        GetTextExtentPoint32A(bf->hDCPrinter, "M", 1, &size);
        rkdiffdiv2 = size.cx;
        SelectObject(hDCPrinter, bf->hFontMainKanji);
        GetTextExtentPoint32A(bf->hDCPrinter, "M", 1, &size);
        rkdiffdiv2 -= size.cx;
#if 0
        rkdiffdiv2 /= 2;
#endif
    }

    if (bf->kanjimode) {
        /* kanjimode */
        /* if yes printout the buffer */
        kanjiasciidiffy = 0;
        i = 0;
        while (i < lstrlenA(bf->text)) {
            SelectObject(hDCPrinter, bf->hFontMainRoman);
            /* calc line size */
            GetTextExtentPoint32A(bf->hDCPrinter, bf->precroman,
                lstrlenA(bf->precroman), &size);
            SelectObject(hDCPrinter, bf->hFontMainKanji);
            if (is_sjis_1stA(bf->text[i])) {
                TextOutA(bf->hDCPrinter,
                    bf->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
                    bf->start_pt.y + kanjiasciidiffy
                    + bf->line * bf->rectangle_pt.y / 66 + DIFFY,
                    &(bf->text[i]), 2);
                lstrcatA(bf->precroman, "MM");
                i += 2;
            } else if (ishankakuA(bf->text[i])) {
                TextOutA(bf->hDCPrinter,
                    bf->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
                    bf->start_pt.y + kanjiasciidiffy
                    + bf->line * bf->rectangle_pt.y / 66 + DIFFY,
                    &(bf->text[i]), 1);
                lstrcatA(bf->precroman, "M");
                i++;
            }
            else {
                /* unknown */
            }
        }
    }    else {
        /* ascii mode */
        SelectObject(hDCPrinter, bf->hFontMainRoman);
        GetTextExtentPoint32A(hDCPrinter, bf->precroman,
            lstrlenA(bf->precroman), &size);
        /* print out */
        kanjiasciidiffy = 0;
        TextOutA(hDCPrinter,
            bf->start_pt.x + size.cx + DIFFX,
            bf->start_pt.y + kanjiasciidiffy
            + bf->line * bf->rectangle_pt.y / 66 + DIFFY,
            bf->text, lstrlenA(bf->text));
        /* add kanji to text buffer */
    }
    SetTextAlign(hDCPrinter, TA_TOP);
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
void buffer_flushW(HWND hWnd, HDC hDCPrinter, BF_W* bfw)
{
  int i;
  int kanjiasciidiffy;
  static SIZE size;
  static int rkdiffdiv2 = 0;
  char chartmp[4];
  WCHAR wchartmp[8];

  SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
  SetTextAlign(hDCPrinter, TA_BASELINE);
  /* zenkaku must stay center of two hankaku */
  if (rkdiffdiv2 == 0) {
    SelectObject(hDCPrinter, bfw->hFontMainRoman);
    GetTextExtentPoint32A(bfw->hDCPrinter, "M", 1, &size);
    rkdiffdiv2 = size.cx;
    SelectObject(hDCPrinter, bfw->hFontMainKanji);
    GetTextExtentPoint32A(bfw->hDCPrinter, "M", 1, &size);
    rkdiffdiv2 -= size.cx;
#if 0
    rkdiffdiv2 /= 2;
#endif
  }

  if (bfw->kanjimode) {
    /* kanjimode */
    /* if yes printout the buffer */
    kanjiasciidiffy = 0;
    i = 0;
    while (i < lstrlenW(bfw->text)) {
      SelectObject(hDCPrinter, bfw->hFontMainRoman);
      /* calc line size */
      GetTextExtentPoint32W(bfw->hDCPrinter, bfw->precroman,
        lstrlenW(bfw->precroman), &size);
      SelectObject(hDCPrinter, bfw->hFontMainKanji);
      if (is_sjis_1stW(bfw->text[i])) {
        chartmp[0] = (char)bfw->text[i];
        chartmp[1] = (char)bfw->text[i+1];
        chartmp[2] = '\0';
        MultiByteToWideChar(932, 0, chartmp, -1, wchartmp, 8);
        //  [in]            UINT                              CodePage,
        //  [in]            DWORD                             dwFlags,
        //  [in]            _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        //  [in]            int                               cbMultiByte,
        //  [out, optional] LPWSTR                            lpWideCharStr,
        //  [in]            int                               cchWideChar

        TextOutW(bfw->hDCPrinter,
          bfw->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
          bfw->start_pt.y + kanjiasciidiffy
          + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
          wchartmp, 1);
        lstrcatW(bfw->precroman, L"MM");
        i += 2;
      }
      else if (ishankakuW(bfw->text[i])) {
        TextOutW(bfw->hDCPrinter,
          bfw->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
          bfw->start_pt.y + kanjiasciidiffy
          + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
          &(bfw->text[i]), 1);
        lstrcatW(bfw->precroman, L"M");
        i++;
      }
      else {
        /* unknown */
      }
    }
  }  else {
    /* ascii mode */
    SelectObject(hDCPrinter, bfw->hFontMainRoman);
    GetTextExtentPoint32W(hDCPrinter, bfw->precroman,
      lstrlenW(bfw->precroman), &size);
    /* print out */
    kanjiasciidiffy = 0;
    TextOutW(hDCPrinter,
      bfw->start_pt.x + size.cx + DIFFX,
      bfw->start_pt.y + kanjiasciidiffy
      + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
      bfw->text, lstrlenW(bfw->text));
    /* add kanji to text buffer */
  }
  SetTextAlign(hDCPrinter, TA_TOP);
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
void buffer_flush_utf8_W(HWND hWnd, HDC hDCPrinter, BF_W* bfw)
{
  int i;
  int kanjiasciidiffy;
  static SIZE size;
  static int rkdiffdiv2 = 0;
  char chartmp[8];
  WCHAR wchartmp[8];

  SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
  SetTextAlign(hDCPrinter, TA_BASELINE);
  /* zenkaku must stay center of two hankaku */
  if (rkdiffdiv2 == 0) {
    SelectObject(hDCPrinter, bfw->hFontMainRoman);
    GetTextExtentPoint32A(bfw->hDCPrinter, "M", 1, &size);
    rkdiffdiv2 = size.cx;
    SelectObject(hDCPrinter, bfw->hFontMainKanji);
    GetTextExtentPoint32A(bfw->hDCPrinter, "M", 1, &size);
    rkdiffdiv2 -= size.cx;
#if 0
    rkdiffdiv2 /= 2;
#endif
  }

  if (bfw->kanjimode) {
    /* kanjimode */
    /* if yes printout the buffer */
    kanjiasciidiffy = 0;
    i = 0;
    while (i < lstrlenW(bfw->text)) {
      SelectObject(hDCPrinter, bfw->hFontMainRoman);
      /* calc line size */
      GetTextExtentPoint32W(bfw->hDCPrinter, bfw->precroman,
        lstrlenW(bfw->precroman), &size);
      SelectObject(hDCPrinter, bfw->hFontMainKanji);
      if (is_utf8_lead_2byteZenW(bfw->text[i])) {
        chartmp[0] = (char)bfw->text[i];
        chartmp[1] = (char)bfw->text[i + 1];
        chartmp[2] = '\0';
        MultiByteToWideChar(CP_UTF8, 0, chartmp, -1, wchartmp, 8);
        //  [in]            UINT                              CodePage,
        //  [in]            DWORD                             dwFlags,
        //  [in]            _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        //  [in]            int                               cbMultiByte,
        //  [out, optional] LPWSTR                            lpWideCharStr,
        //  [in]            int                               cchWideChar

        TextOutW(bfw->hDCPrinter,
          bfw->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
          bfw->start_pt.y + kanjiasciidiffy
          + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
          wchartmp, 1);
        if (is_utf8_2byte_hankakuW(bfw->text[i], bfw->text[i + 1])) {
          lstrcatW(bfw->precroman, L"M");
        }        else {
          lstrcatW(bfw->precroman, L"MM");
        }
          i += 2;
      }      else if (is_utf8_lead_3byteZenW(bfw->text[i])) {
        chartmp[0] = (char)bfw->text[i];
        chartmp[1] = (char)bfw->text[i + 1];
        chartmp[2] = (char)bfw->text[i + 2];
        chartmp[3] = '\0';
        MultiByteToWideChar(CP_UTF8, 0, chartmp, -1, wchartmp, 8);
        //  [in]            UINT                              CodePage,
        //  [in]            DWORD                             dwFlags,
        //  [in]            _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        //  [in]            int                               cbMultiByte,
        //  [out, optional] LPWSTR                            lpWideCharStr,
        //  [in]            int                               cchWideChar
        TextOutW(bfw->hDCPrinter,
          bfw->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
          bfw->start_pt.y + kanjiasciidiffy
          + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
          wchartmp, 1);
        if (is_utf8_3byte_hankakuW(bfw->text[i], bfw->text[i + 1], bfw->text[i + 2])) {
          lstrcatW(bfw->precroman, L"M");
        }
        else {
          lstrcatW(bfw->precroman, L"MM");
        }

        i += 3;
      }
      else if (is_utf8_lead_4byteZenW(bfw->text[i])) {
        chartmp[0] = (char)bfw->text[i];
        chartmp[1] = (char)bfw->text[i + 1];
        chartmp[2] = (char)bfw->text[i + 2];
        chartmp[3] = (char)bfw->text[i + 3];
        chartmp[4] = '\0';
        MultiByteToWideChar(CP_UTF8, 0, chartmp, -1, wchartmp, 8);
        //  [in]            UINT                              CodePage,
        //  [in]            DWORD                             dwFlags,
        //  [in]            _In_NLS_string_(cbMultiByte)LPCCH lpMultiByteStr,
        //  [in]            int                               cbMultiByte,
        //  [out, optional] LPWSTR                            lpWideCharStr,
        //  [in]            int                               cchWideChar
        TextOutW(bfw->hDCPrinter,
          bfw->start_pt.x + size.cx + DIFFX + rkdiffdiv2,
          bfw->start_pt.y + kanjiasciidiffy
          + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
          wchartmp, 1);
        lstrcatW(bfw->precroman, L"MM");
        i += 4;
      }      else {
        /* unknown */
      }
    }
  }  else {
    /* ascii mode */
    SelectObject(hDCPrinter, bfw->hFontMainRoman);
    GetTextExtentPoint32W(hDCPrinter, bfw->precroman,
      lstrlenW(bfw->precroman), &size);
    /* print out */
    kanjiasciidiffy = 0;
    TextOutW(hDCPrinter,
      bfw->start_pt.x + size.cx + DIFFX,
      bfw->start_pt.y + kanjiasciidiffy
      + bfw->line * bfw->rectangle_pt.y / 66 + DIFFY,
      bfw->text, lstrlenW(bfw->text));
    /* add kanji to text buffer */
  }
  SetTextAlign(hDCPrinter, TA_TOP);
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/* output lines */
/* page should be 1 or 2 */
void drawlines(int page, HDC hDCPrinter)
{
    static HPEN hPen, hOldPen;
    static POINT pt;

    SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */

    hPen = CreatePen(PS_SOLID, 40, RGB(0, 0, 0));
    hOldPen = (HPEN)SelectObject(hDCPrinter, hPen);

    switch (page) {
    case 1:
        MoveToEx(hDCPrinter, LINE_LEFT1 + DIFFX, LINE_UPPER + DIFFY, &pt);
        LineTo(hDCPrinter, LINE_RIGHT1 + DIFFX, LINE_UPPER + DIFFY);
        LineTo(hDCPrinter, LINE_RIGHT1 + DIFFX, LINE_LOWER + DIFFY);
        LineTo(hDCPrinter, LINE_LEFT1 + DIFFX, LINE_LOWER + DIFFY);
        LineTo(hDCPrinter, LINE_LEFT1 + DIFFX, LINE_UPPER + DIFFY);
        /* draw a line under header */
        MoveToEx(hDCPrinter, LINE_LEFT1 + DIFFX, LINE_TITLE + DIFFY, &pt);
        LineTo(hDCPrinter, LINE_RIGHT1 + DIFFX, LINE_TITLE + DIFFY);
        break;
    case 2:
        MoveToEx(hDCPrinter, LINE_LEFT2 + DIFFX, LINE_UPPER + DIFFY, &pt);
        LineTo(hDCPrinter, LINE_RIGHT2 + DIFFX, LINE_UPPER + DIFFY);
        LineTo(hDCPrinter, LINE_RIGHT2 + DIFFX, LINE_LOWER + DIFFY);
        LineTo(hDCPrinter, LINE_LEFT2 + DIFFX, LINE_LOWER + DIFFY);
        LineTo(hDCPrinter, LINE_LEFT2 + DIFFX, LINE_UPPER + DIFFY);
        /* draw a line under header */
        MoveToEx(hDCPrinter, LINE_LEFT2 + DIFFX, LINE_TITLE + DIFFY, &pt);
        LineTo(hDCPrinter, LINE_RIGHT2 + DIFFX, LINE_TITLE + DIFFY);
        break;
    }

    SelectObject(hDCPrinter, hOldPen);
    DeleteObject(hPen);
}

/*----------------------------------------------------------------------*/
void print_form(HWND hWnd, HDC hDCPrinter, SYSTEMTIME* systemtime,
    int side, int page, WCHAR* filename)
{
    static LOGFONT lf;
    int datex, pagex, titlex;
    static HFONT hFontDate, hFontOrg, hFontPage, hFontTitle;
    static WCHAR cmonth[12][4] = { L"Jan", L"Feb", L"Mar", L"Apr",
        L"May", L"Jun", L"Jul", L"Aug",
        L"Sep", L"Oct", L"Nov", L"Dec" };
    static WCHAR cdate[256], ctitle[256];
    static WCHAR cpage[8];
    static SIZE size_title, size_date, size_pagenum, size_space;
    int title_space;

    SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
    SetTextAlign(hDCPrinter, TA_BASELINE);

    /* date font is 12 x 0.8pt = 9.6 = 96 x 254 / 72 (0.01mm) */
    lf.lfHeight = -96 * 254 / 72;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = 400;
    lf.lfItalic = 0;
    lf.lfUnderline = 0;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = 0;
    lf.lfOutPrecision = 3;
    lf.lfClipPrecision = 2;
    lf.lfQuality = 1;
    lf.lfPitchAndFamily = 34;
    lstrcpy(lf.lfFaceName, L"Helvetica");

    /* page 1 */
    /* start printing header */
    /* date/time */
    hFontDate = CreateFontIndirect(&lf);
    hFontOrg = (HFONT)SelectObject(hDCPrinter, hFontDate);
    wsprintf(cdate, L"%s %d %d %02d:%02d",
        cmonth[systemtime->wMonth - 1],
        systemtime->wDay,
        systemtime->wYear,
        systemtime->wHour,
        systemtime->wMinute);

    if (side == 1) {
        datex = DATEX1;
        pagex = PAGEX1;
        /* center of the title */
        titlex = TITLEX1;
    }
    else {
        datex = DATEX2;
        pagex = PAGEX2;
        titlex = TITLEX2;
    }

    GetTextExtentPoint32(hDCPrinter, cdate, lstrlen(cdate), &size_date);
    /* left and upper side */
    TextOut(hDCPrinter, datex + DIFFX, PAGEY + DIFFY, cdate, lstrlen(cdate));
    hFontOrg = (HFONT)SelectObject(hDCPrinter, hFontOrg);
    DeleteObject(hFontDate);

    /* page */
    hFontPage = CreateFontIndirect(&lf);
    hFontOrg = (HFONT)SelectObject(hDCPrinter, hFontPage);
    wsprintf(cpage, L"Page %d", page);
    GetTextExtentPoint32(hDCPrinter, cpage, lstrlen(cpage), &size_pagenum);
    GetTextExtentPoint32(hDCPrinter, L"MMMM", lstrlen(L"MMMM"), &size_space);
    TextOut(hDCPrinter, pagex + DIFFX, PAGEY + DIFFY, cpage, lstrlen(cpage));
    if (side == 1) {
        wsprintf(cpage, L"%d", (page + 1) / 2);
        TextOut(hDCPrinter, FOOTPAGEX + DIFFX, FOOTTITLEY + DIFFY,
            cpage, lstrlen(cpage));
    }

    SelectObject(hDCPrinter, hFontOrg);
    DeleteObject(hFontDate);

    /* title */
    /* compare title space and title width */
    /* if title space < title width then change font size */

    /* title font is 12pt = 120 x 254 / 72 (0.01mm) */
    lf.lfHeight = -120 * 254 / 72;
    lf.lfWeight = 700;
    lf.lfOutPrecision = 3;
    lf.lfClipPrecision = 2;
    lf.lfQuality = 1;
    lf.lfPitchAndFamily = 34;
    lstrcpy(lf.lfFaceName, L"Helvetica");

    hFontTitle = CreateFontIndirect(&lf);
    hFontOrg = (HFONT)SelectObject(hDCPrinter, hFontTitle);

    lstrcpy(ctitle, filename);

    GetTextExtentPoint32(hDCPrinter, ctitle, lstrlen(ctitle), &size_title);
    title_space = pagex - (datex + size_date.cx) - size_space.cx;
    SelectObject(hDCPrinter, hFontOrg);
    DeleteObject(hFontTitle);
    if (title_space < size_title.cx) {
        /* change font size */
        /* title font is 12pt = 120 x 254 / 72 (0.01mm) */
        lf.lfHeight = -120 * 254 * title_space / 72 / size_title.cx;
    }
    hFontTitle = CreateFontIndirect(&lf);
    hFontOrg = (HFONT)SelectObject(hDCPrinter, hFontTitle);
    GetTextExtentPoint32(hDCPrinter, ctitle, lstrlen(ctitle), &size_title);
    TextOut(hDCPrinter, titlex - (size_title.cx / 2) + DIFFX, TITLEY + DIFFY,
        ctitle, lstrlen(ctitle));
    SelectObject(hDCPrinter, hFontOrg);
    if (side == 1) {
        lf.lfHeight = -96 * 254 / 72;
        hFontTitle = CreateFontIndirect(&lf);
        hFontOrg = (HFONT)SelectObject(hDCPrinter, hFontTitle);
        TextOut(hDCPrinter, FOOTTITLEX + DIFFX, FOOTTITLEY + DIFFY,
            ctitle, lstrlen(ctitle));
    }
    SetTextAlign(hDCPrinter, TA_TOP);

    SelectObject(hDCPrinter, hFontOrg);
    DeleteObject(hFontTitle);

    drawlines(side, hDCPrinter);
}

/*----------------------------------------------------------------------*/
LRESULT WINAPI OutTextA(HWND hWnd, LPVOID lpVoid, HDC hDCPrinter, WCHAR* filename)
{
    int page = 0;
    int fromleft = 0;
    int i;
    static HFONT hFontOrg;
    static LOGFONT lf;
    SYSTEMTIME systemtime;
    int side = 1; /* is page left(=1) or right(=2)? */
    int currentpoint = 0;
    static BF bf;
    static HPEN hPen, hOldPen;
    static unsigned char* lpStr;
    int tab;
    static SIZE size;
    int orikaeshi;

    ZeroMemory(&bf, sizeof(bf));
    ZeroMemory(&lf, sizeof(lf));

    bf.hDCPrinter = hDCPrinter;

    lpStr = (unsigned char*)lpVoid;
    SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
    GetLocalTime(&systemtime);

    /* main font is 6.6pt = 66 x 254 / 72 (0.01mm) */
    lf.lfHeight = -66 * 254 / 72;
    lf.lfWeight = 400;
    lf.lfOutPrecision = 3;
    lf.lfClipPrecision = 2;
    lf.lfQuality = 1;
    lf.lfPitchAndFamily = 49;
    lstrcpy(lf.lfFaceName, L"Courier");

    bf.hFontMainRoman = CreateFontIndirect(&lf);

    /* ryumin */
    /* main font is 6.6pt = 66 x 254 / 72 (0.01mm) */
    lf.lfHeight = -66 * 254 / 72;
    lf.lfWeight = 250;
    lf.lfOutPrecision = 3;
    lf.lfClipPrecision = 2;
    lf.lfQuality = 1;
    lf.lfPitchAndFamily = 17;
    lf.lfCharSet = 128;
    //lstrcpy(lf.lfFaceName, L"リュウミンL-KL");
    lstrcpy(lf.lfFaceName, L"Meiryo UI");

    if (NULL == (bf.hFontMainKanji = CreateFontIndirect(&lf))) {
      MessageBox(NULL, L"CreateFontIndirect Kanji Fail.", L"DEBUG", MB_OK);
    }

    hFontOrg = (HFONT)SelectObject(hDCPrinter, bf.hFontMainRoman);

    /* nennotame */
#if 0
    lstrcpy(bf.text, "");
    lstrcpy(bf.croman, "");
    lstrcpy(bf.precroman, "");
#endif
    bf.text[0] = L'\0';
    bf.croman[0] = L'\0';
    bf.precroman[0] = L'\0';

    while (NULL != lpStr[currentpoint]) {
        SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
        if (bf.line == 0 && fromleft == 0) {
            if (side == 1) {
                StartPage(hDCPrinter);
            }
            page++;
            print_form(hWnd, hDCPrinter, &systemtime, side, page, filename);
        }

        /* calc rectangle of the main text body. */
        if (side == 1) {
            /* start printing out lines */
            bf.start_pt.x = START_PTX1; bf.start_pt.y = START_PTY;
            /* get heights of rectangle */
            bf.end_pt.x = START_PTX1; bf.end_pt.y = END_PTY;
            orikaeshi = ORIKAESHI1;
        }
        else {
            /* start printing out lines */
            bf.start_pt.x = START_PTX2; bf.start_pt.y = START_PTY;
            /* get heights of rectangle */
            bf.end_pt.x = START_PTX2; bf.end_pt.y = END_PTY;;
            orikaeshi = ORIKAESHI2;
        }
        /* divide them by 66 makes a line height */
        bf.rectangle_pt.x = bf.end_pt.x - bf.start_pt.x;
        bf.rectangle_pt.y = bf.end_pt.y - bf.start_pt.y;

        /* must consider line length exceeds rectangle.x */

        /* get a letter */
        /* is sjis first?? */
        /* yes then get a second letter, no then store in text buffer */
        /* check CR/LF */
        bf.c[0] = lpStr[currentpoint];
        /* last letter of lpVoid is NULL */
        if (bf.c[0] != 0) {
            bf.c[1] = lpStr[currentpoint + 1];
        }
        else {
            bf.c[1] = 0;
        }

        if (bf.c[0] == 0x0d && bf.c[1] == 0x0a) {
            /* dos's return */
            /* print a line */
            buffer_flushA(hWnd, hDCPrinter, &bf);
            fromleft = 0;
            bf.line++;
            currentpoint += 2;
            /* clear text buffer */
#if 0
            lstrcpy(bf.text, "");
            lstrcpy(bf.croman, "");
            lstrcpy(bf.precroman, "");
#endif
            bf.text[0] = '\0';
            bf.croman[0] = '\0';
            bf.precroman[0] = '\0';
        }
        else if (bf.c[0] == 0x0d || bf.c[0] == 0x0a) {
            /* mac's or unix's return */
            /* print a line */
            buffer_flushA(hWnd, hDCPrinter, &bf);
            lstrcpyA(bf.text, "");
            lstrcpyA(bf.precroman, bf.croman);
            fromleft = 0;
            bf.line++;
            currentpoint++;
        }
        else if (bf.c[0] == 9) {
            /* tab */
            /* if no printout the buffer */
            /* previous mode is ascii */
            /* get width of the text */
            buffer_flushA(hWnd, hDCPrinter, &bf);
            lstrcpyA(bf.text, "");
            lstrcpyA(bf.precroman, bf.croman);
            bf.kanjimode = 0;
            /* check length after each replace and 8 char */
            tab = 8 - (lstrlenA(bf.croman) % 8);
            i = 0;
            bf.c[0] = ' ';
            bf.c[1] = '\0';
            while (i < tab) {
                lstrcatA(bf.text, bf.c);
                fromleft++;
                lstrcatA(bf.croman, "M");
                /* check length */
                SelectObject(hDCPrinter, bf.hFontMainRoman);
                GetTextExtentPoint32A(hDCPrinter, bf.croman,
                    lstrlenA(bf.croman), &size);
                if (bf.start_pt.x + size.cx > orikaeshi) {
                    buffer_flushA(hWnd, hDCPrinter, &bf);
                    fromleft = 0;
                    bf.line++;
                    /* clear text buffer */
#if 0
                    lstrcpy(bf.text, "");
                    lstrcpy(bf.croman, "");
                    lstrcpy(bf.precroman, "");
#endif
                    bf.text[0] = '\0';
                    bf.croman[0] = '\0';
                    bf.precroman[0] = '\0';
                }
                i++;
            }
            currentpoint++;
        }
        else if (is_sjis_1stA((unsigned char)bf.c[0])) {
            /* yes sjis 1st */
            /* previous state kanji?? */
            if (bf.kanjimode) {
                /* if yes add the letter at the last */
            }
            else {
                /* if no printout the buffer */
                /* previous mode is ascii */
                /* get width of the text */
                buffer_flushA(hWnd, hDCPrinter, &bf);
                lstrcpyA(bf.text, "");
                lstrcpyA(bf.precroman, bf.croman);
            }
            bf.c[2] = '\0';
            lstrcatA(bf.text, bf.c);
            fromleft += 2;
            currentpoint += 2;
            lstrcatA(bf.croman, "MM");
            bf.kanjimode = 1;
            /* check length */
            SelectObject(hDCPrinter, bf.hFontMainRoman);
            GetTextExtentPoint32A(hDCPrinter, bf.croman,
                lstrlenA(bf.croman), &size);
            if (bf.start_pt.x + size.cx > orikaeshi) {
                buffer_flushA(hWnd, hDCPrinter, &bf);
                fromleft = 0;
                bf.line++;
                /* clear text buffer */
#if 0
                lstrcpy(bf.text, "");
                lstrcpy(bf.croman, "");
                lstrcpy(bf.precroman, "");
#endif
                bf.text[0] = '\0';
                bf.croman[0] = '\0';
                bf.precroman[0] = '\0';
            }
        }
        else if (ishankakuA(bf.c[0])) {
            /* yes hankaku */
            /* previous state kanji?? */
            if (bf.kanjimode) {
                /* if yes add the letter at the last */
            }
            else {
                /* if no printout the buffer */
                /* previous mode is ascii */
                /* get width of the text */
                buffer_flushA(hWnd, hDCPrinter, &bf);
                lstrcpyA(bf.text, "");
                lstrcpyA(bf.precroman, bf.croman);
            }
            bf.c[1] = '\0';
            lstrcatA(bf.text, bf.c);
            fromleft++;
            currentpoint++;
            lstrcatA(bf.croman, "M");
            bf.kanjimode = 1;
            /* check length */
            SelectObject(hDCPrinter, bf.hFontMainRoman);
            GetTextExtentPoint32A(hDCPrinter, bf.croman,
                lstrlenA(bf.croman), &size);
            if (bf.start_pt.x + size.cx > orikaeshi) {
                buffer_flushA(hWnd, hDCPrinter, &bf);
                fromleft = 0;
                bf.line++;
                /* clear text buffer */
#if 0
                lstrcpy(bf.text, "");
                lstrcpy(bf.croman, "");
                lstrcpy(bf.precroman, "");
#endif
                bf.text[0] = '\0';
                bf.croman[0] = '\0';
                bf.precroman[0] = '\0';
            }
        }
        else {
            /* ascii */
            /* previous state kanji?? */
            if (bf.kanjimode) {
                /* if yes, flush text buffer */
                buffer_flushA(hWnd, hDCPrinter, &bf);
                lstrcpyA(bf.text, "");
                lstrcpyA(bf.precroman, bf.croman);
            }
            else {
                /* if no add the letter at the last */
            }
            bf.c[1] = '\0';
            lstrcatA(bf.text, bf.c);
            fromleft++;
            currentpoint++;
            lstrcatA(bf.croman, "M");
            bf.kanjimode = 0;
            /* check length */
            SelectObject(hDCPrinter, bf.hFontMainRoman);
            GetTextExtentPoint32A(hDCPrinter, bf.croman,
                lstrlenA(bf.croman), &size);
            if (bf.start_pt.x + size.cx > orikaeshi) {
                buffer_flushA(hWnd, hDCPrinter, &bf);
                fromleft = 0;
                bf.line++;
                /* clear text buffer */
#if 0
                lstrcpy(bf.text, "");
                lstrcpy(bf.croman, "");
                lstrcpy(bf.precroman, "");
#endif
                bf.text[0] = '\0';
                bf.croman[0] = '\0';
                bf.precroman[0] = '\0';

            }
        }
        if (bf.line >= 66) {
            if (side == 1) {
                side = 2;
            }
            else {
                EndPage(hDCPrinter);
                side = 1;
            }
            bf.line = 0;
        }
    }

    /* need final flush */
    buffer_flushA(hWnd, hDCPrinter, &bf);
    lstrcpyA(bf.text, "");
    /* need endpage??? */
    EndPage(hDCPrinter);

    SelectObject(hDCPrinter, hFontOrg);
    DeleteObject(bf.hFontMainRoman);
    DeleteObject(bf.hFontMainKanji);

    SelectObject(hDCPrinter, hOldPen);
    DeleteObject(hPen);

    return TRUE;
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
LRESULT WINAPI OutTextW(HWND hWnd, LPVOID lpVoid, HDC hDCPrinter, WCHAR* filename)
{
  int page = 0;
  int fromleft = 0;
  int i;
  static HFONT hFontOrg;
  static LOGFONT lf;
  SYSTEMTIME systemtime;
  int side = 1; /* is page left(=1) or right(=2)? */
  int currentpoint = 0;
  static BF_W bfw;
  static HPEN hPen, hOldPen;
  static unsigned char* lpStr;
  int tab;
  static SIZE size;
  int orikaeshi;

  ZeroMemory(&bfw, sizeof(bfw));
  ZeroMemory(&lf, sizeof(lf));

  bfw.hDCPrinter = hDCPrinter;

  lpStr = (unsigned char*)lpVoid;
  SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
  GetLocalTime(&systemtime);

  /* main font is 6.6pt = 66 x 254 / 72 (0.01mm) */
  lf.lfHeight = -66 * 254 / 72;
  lf.lfWeight = 400;
  lf.lfOutPrecision = 3;
  lf.lfClipPrecision = 2;
  lf.lfQuality = 1;
  lf.lfPitchAndFamily = 49;
  lstrcpyW(lf.lfFaceName, L"Courier");

  bfw.hFontMainRoman = CreateFontIndirect(&lf);

//#define ANSI_CHARSET 0                  'Windows標準文字セット
//#define DEFAULT_CHARSET 1               'デフォルト(指定なし)
//#define SYMBOL_CHARSET 2                'シンボル文字セット
//#define MAC_CHARSET 77                  ' 
//#define SHIFTJIS_CHARSET 128            'シフトJIS文字セット
//#define HANGEUL_CHARSET 129             'ハングル文字セット
//#define JOHAB_CHARSET 130               '韓国版Windows文字セット
//#define GB2312_CHARSET 134              '
//#define CHINESEBIG5_CHARSET 136         '中国語文字セット
//#define GREEK_CHARSET 161               '
//#define TURKISH_CHARSET 162             '
//#define VIETNAMESE_CHARSET 163          '
//#define HEBREW_CHARSET 177              '中東語版Windows(ヘブライ語)文字セット
//#define ARABIC_CHARSET 178              '中東語版Windows(アラビア語)文字セット
//#define BALTIC_CHARSET 186              '
//#define RUSSIAN_CHARSET 204             '
//#define THAI_CHARSET 222                'タイ版Windows文字セット
//#define EASTEUROPE_CHARSET 238          '
//#define OEM_CHARSET 255                 'OEM文字セット
  ZeroMemory(&lf, sizeof(lf));

  /* ryumin */
  /* main font is 6.6pt = 66 x 254 / 72 (0.01mm) */
  lf.lfHeight = -66 * 254 / 72;
  //lf.lfWidth = -66 * 254 / 72;
  lf.lfWeight = 250;
  lf.lfOutPrecision = 3;
  lf.lfClipPrecision = 2;
  lf.lfQuality = 1;
  lf.lfPitchAndFamily = 17;
  lf.lfCharSet = 128;
  //lstrcpyW(lf.lfFaceName, L"游ゴシック");
  //lstrcpyW(lf.lfFaceName, L"Meiryo UI");
  //lstrcpyW(lf.lfFaceName, L"ＭＳ ゴシック");
  lstrcpyW(lf.lfFaceName, L"リュウミンL-KL");

  if (NULL == (bfw.hFontMainKanji = CreateFontIndirect(&lf))) {
    MessageBox(NULL, L"CreateFontIndirect Kanji Fail.", L"DEBUG", MB_OK);
  }

  hFontOrg = (HFONT)SelectObject(hDCPrinter, bfw.hFontMainRoman);

  /* nennotame */
#if 0
  lstrcpy(bf.text, "");
  lstrcpy(bf.croman, "");
  lstrcpy(bf.precroman, "");
#endif
  bfw.text[0] = L'\0';
  bfw.croman[0] = L'\0';
  bfw.precroman[0] = L'\0';

  while (NULL != lpStr[currentpoint]) {
    SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
    if (bfw.line == 0 && fromleft == 0) {
      if (side == 1) {
        StartPage(hDCPrinter);
      }
      page++;
      print_form(hWnd, hDCPrinter, &systemtime, side, page, filename);
    }

    /* calc rectangle of the main text body. */
    if (side == 1) {
      /* start printing out lines */
      bfw.start_pt.x = START_PTX1; bfw.start_pt.y = START_PTY;
      /* get heights of rectangle */
      bfw.end_pt.x = START_PTX1; bfw.end_pt.y = END_PTY;
      orikaeshi = ORIKAESHI1;
    }    else {
      /* start printing out lines */
      bfw.start_pt.x = START_PTX2; bfw.start_pt.y = START_PTY;
      /* get heights of rectangle */
      bfw.end_pt.x = START_PTX2; bfw.end_pt.y = END_PTY;;
      orikaeshi = ORIKAESHI2;
    }
    /* divide them by 66 makes a line height */
    bfw.rectangle_pt.x = bfw.end_pt.x - bfw.start_pt.x;
    bfw.rectangle_pt.y = bfw.end_pt.y - bfw.start_pt.y;

    /* must consider line length exceeds rectangle.x */

    /* get a letter */
    /* is sjis first?? */
    /* yes then get a second letter, no then store in text buffer */
    /* check CR/LF */
    bfw.c[0] = lpStr[currentpoint];
    /* last letter of lpVoid is NULL */
    if (bfw.c[0] != 0) {
      bfw.c[1] = lpStr[currentpoint + 1];
    }
    else {
      bfw.c[1] = 0;
    }

    if (bfw.c[0] == 0x0d && bfw.c[1] == 0x0a) {
      /* dos's return */
      /* print a line */
      buffer_flushW(hWnd, hDCPrinter, &bfw);
      fromleft = 0;
      bfw.line++;
      currentpoint += 2;
      /* clear text buffer */
#if 0
      lstrcpy(bf.text, "");
      lstrcpy(bf.croman, "");
      lstrcpy(bf.precroman, "");
#endif
      bfw.text[0] = '\0';
      bfw.croman[0] = '\0';
      bfw.precroman[0] = '\0';
    }    else if (bfw.c[0] == 0x0d || bfw.c[0] == 0x0a) {
      /* mac's or unix's return */
      /* print a line */
      buffer_flushW(hWnd, hDCPrinter, &bfw);
      lstrcpyW(bfw.text, L"");
      lstrcpyW(bfw.precroman, bfw.croman);
      fromleft = 0;
      bfw.line++;
      currentpoint++;
    }    else if (bfw.c[0] == 9) {
      /* tab */
      /* if no printout the buffer */
      /* previous mode is ascii */
      /* get width of the text */
      buffer_flushW(hWnd, hDCPrinter, &bfw);
      lstrcpyW(bfw.text, L"");
      lstrcpyW(bfw.precroman, bfw.croman);
      bfw.kanjimode = 0;
      /* check length after each replace and 8 char */
      tab = 8 - (lstrlenW(bfw.croman) % 8);
      i = 0;
      bfw.c[0] = L' ';
      bfw.c[1] = L'\0';
      while (i < tab) {
        lstrcatW(bfw.text, bfw.c);
        fromleft++;
        lstrcatW(bfw.croman, L"M");
        /* check length */
        SelectObject(hDCPrinter, bfw.hFontMainRoman);
        GetTextExtentPoint32W(hDCPrinter, bfw.croman,
          lstrlenW(bfw.croman), &size);
        if (bfw.start_pt.x + size.cx > orikaeshi) {
          buffer_flushW(hWnd, hDCPrinter, &bfw);
          fromleft = 0;
          bfw.line++;
          /* clear text buffer */
#if 0
          lstrcpy(bf.text, "");
          lstrcpy(bf.croman, "");
          lstrcpy(bf.precroman, "");
#endif
          bfw.text[0] = L'\0';
          bfw.croman[0] = L'\0';
          bfw.precroman[0] = L'\0';
        }
        i++;
      }
      currentpoint++;
    } else if (is_sjis_1stW(bfw.c[0])) {
      /* yes sjis 1st */
      /* previous state kanji?? */
      if (bfw.kanjimode) {
        /* if yes add the letter at the last */
      }
      else {
        /* if no printout the buffer */
        /* previous mode is ascii */
        /* get width of the text */
        buffer_flushW(hWnd, hDCPrinter, &bfw);
        lstrcpyW(bfw.text, L"");
        lstrcpyW(bfw.precroman, bfw.croman);
      }
      bfw.c[2] = '\0';
      lstrcatW(bfw.text, bfw.c);
      fromleft += 2;
      currentpoint += 2;
      lstrcatW(bfw.croman, L"MM");
      bfw.kanjimode = 1;
      /* check length */
      SelectObject(hDCPrinter, bfw.hFontMainRoman);
      GetTextExtentPoint32W(hDCPrinter, bfw.croman,
        lstrlenW(bfw.croman), &size);
      if (bfw.start_pt.x + size.cx > orikaeshi) {
        buffer_flushW(hWnd, hDCPrinter, &bfw);
        fromleft = 0;
        bfw.line++;
        /* clear text buffer */
#if 0
        lstrcpy(bf.text, "");
        lstrcpy(bf.croman, "");
        lstrcpy(bf.precroman, "");
#endif
        bfw.text[0] = '\0';
        bfw.croman[0] = '\0';
        bfw.precroman[0] = '\0';
      }
    }
    else if (ishankakuW(bfw.c[0])) {
      /* yes hankaku */
      /* previous state kanji?? */
      if (bfw.kanjimode) {
        /* if yes add the letter at the last */
      }
      else {
        /* if no printout the buffer */
        /* previous mode is ascii */
        /* get width of the text */
        buffer_flushW(hWnd, hDCPrinter, &bfw);
        lstrcpyW(bfw.text, L"");
        lstrcpyW(bfw.precroman, bfw.croman);
      }
      bfw.c[1] = L'\0';
      lstrcatW(bfw.text, bfw.c);
      fromleft++;
      currentpoint++;
      lstrcatW(bfw.croman, L"M");
      bfw.kanjimode = 1;
      /* check length */
      SelectObject(hDCPrinter, bfw.hFontMainRoman);
      GetTextExtentPoint32W(hDCPrinter, bfw.croman,
        lstrlenW(bfw.croman), &size);
      if (bfw.start_pt.x + size.cx > orikaeshi) {
        buffer_flushW(hWnd, hDCPrinter, &bfw);
        fromleft = 0;
        bfw.line++;
        /* clear text buffer */
#if 0
        lstrcpy(bf.text, "");
        lstrcpy(bf.croman, "");
        lstrcpy(bf.precroman, "");
#endif
        bfw.text[0] = L'\0';
        bfw.croman[0] = L'\0';
        bfw.precroman[0] = L'\0';
      }
    }    else {
      /* ascii */
      /* previous state kanji?? */
      if (bfw.kanjimode) {
        /* if yes, flush text buffer */
        buffer_flushW(hWnd, hDCPrinter, &bfw);
        lstrcpyW(bfw.text, L"");
        lstrcpyW(bfw.precroman, bfw.croman);
      }      else {
        /* if no add the letter at the last */
      }
      bfw.c[1] = L'\0';
      lstrcatW(bfw.text, bfw.c);
      fromleft++;
      currentpoint++;
      lstrcatW(bfw.croman, L"M");
      bfw.kanjimode = 0;
      /* check length */
      SelectObject(hDCPrinter, bfw.hFontMainRoman);
      GetTextExtentPoint32W(hDCPrinter, bfw.croman,
        lstrlenW(bfw.croman), &size);
      if (bfw.start_pt.x + size.cx > orikaeshi) {
        buffer_flushW(hWnd, hDCPrinter, &bfw);
        fromleft = 0;
        bfw.line++;
        /* clear text buffer */
#if 0
        lstrcpy(bf.text, "");
        lstrcpy(bf.croman, "");
        lstrcpy(bf.precroman, "");
#endif
        bfw.text[0] = '\0';
        bfw.croman[0] = '\0';
        bfw.precroman[0] = '\0';

      }
    }
    if (bfw.line >= 66) {
      if (side == 1) {
        side = 2;
      }
      else {
        EndPage(hDCPrinter);
        side = 1;
      }
      bfw.line = 0;
    }
  }

  /* need final flush */
  buffer_flushW(hWnd, hDCPrinter, &bfw);
  lstrcpyW(bfw.text, L"");
  /* need endpage??? */
  EndPage(hDCPrinter);

  SelectObject(hDCPrinter, hFontOrg);
  DeleteObject(bfw.hFontMainRoman);
  DeleteObject(bfw.hFontMainKanji);

  SelectObject(hDCPrinter, hOldPen);
  DeleteObject(hPen);

  return TRUE;
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
LRESULT WINAPI OutTextUTF8W(HWND hWnd, LPVOID lpVoid, HDC hDCPrinter, WCHAR* filename)
{
  int page = 0;
  int fromleft = 0;
  int i;
  static HFONT hFontOrg;
  static LOGFONT lf;
  SYSTEMTIME systemtime;
  int side = 1; /* is page left(=1) or right(=2)? */
  int currentpoint = 0;
  static BF_W bfw;
  static HPEN hPen, hOldPen;
  static unsigned char* lpStr;
  int tab;
  static SIZE size;
  int orikaeshi;
  WCHAR stdmsg[256] = { 0 };

  ZeroMemory(&bfw, sizeof(bfw));
  ZeroMemory(&lf, sizeof(lf));

  bfw.hDCPrinter = hDCPrinter;

  lpStr = (unsigned char*)lpVoid;
  SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
  GetLocalTime(&systemtime);

  /* main font is 6.6pt = 66 x 254 / 72 (0.01mm) */
  lf.lfHeight = -66 * 254 / 72;
  lf.lfWeight = 400;
  lf.lfOutPrecision = 3;
  lf.lfClipPrecision = 2;
  lf.lfQuality = 1;
  lf.lfPitchAndFamily = 49;
  lstrcpyW(lf.lfFaceName, L"Courier");

  bfw.hFontMainRoman = CreateFontIndirect(&lf);

  //#define ANSI_CHARSET 0                  'Windows標準文字セット
  //#define DEFAULT_CHARSET 1               'デフォルト(指定なし)
  //#define SYMBOL_CHARSET 2                'シンボル文字セット
  //#define MAC_CHARSET 77                  ' 
  //#define SHIFTJIS_CHARSET 128            'シフトJIS文字セット
  //#define HANGEUL_CHARSET 129             'ハングル文字セット
  //#define JOHAB_CHARSET 130               '韓国版Windows文字セット
  //#define GB2312_CHARSET 134              '
  //#define CHINESEBIG5_CHARSET 136         '中国語文字セット
  //#define GREEK_CHARSET 161               '
  //#define TURKISH_CHARSET 162             '
  //#define VIETNAMESE_CHARSET 163          '
  //#define HEBREW_CHARSET 177              '中東語版Windows(ヘブライ語)文字セット
  //#define ARABIC_CHARSET 178              '中東語版Windows(アラビア語)文字セット
  //#define BALTIC_CHARSET 186              '
  //#define RUSSIAN_CHARSET 204             '
  //#define THAI_CHARSET 222                'タイ版Windows文字セット
  //#define EASTEUROPE_CHARSET 238          '
  //#define OEM_CHARSET 255                 'OEM文字セット
  ZeroMemory(&lf, sizeof(lf));

  /* ryumin */
  /* main font is 6.6pt = 66 x 254 / 72 (0.01mm) */
  lf.lfHeight = -66 * 254 / 72;
  //lf.lfWidth = -66 * 254 / 72;
  lf.lfWeight = 250;
  lf.lfOutPrecision = 3;
  lf.lfClipPrecision = 2;
  lf.lfQuality = 1;
  lf.lfPitchAndFamily = 17;
  lf.lfCharSet = 128;
  //lstrcpyW(lf.lfFaceName, L"游ゴシック");
  //lstrcpyW(lf.lfFaceName, L"Meiryo UI");
  //lstrcpyW(lf.lfFaceName, L"ＭＳ ゴシック");
  lstrcpyW(lf.lfFaceName, L"リュウミンL-KL");

  if (NULL == (bfw.hFontMainKanji = CreateFontIndirect(&lf))) {
    MessageBox(NULL, L"CreateFontIndirect Kanji Fail.", L"DEBUG", MB_OK);
  }

  hFontOrg = (HFONT)SelectObject(hDCPrinter, bfw.hFontMainRoman);

  /* nennotame */
#if 0
  lstrcpy(bf.text, "");
  lstrcpy(bf.croman, "");
  lstrcpy(bf.precroman, "");
#endif
  bfw.text[0] = L'\0';
  bfw.croman[0] = L'\0';
  bfw.precroman[0] = L'\0';

  while (NULL != lpStr[currentpoint]) {
    SetMapMode(hDCPrinter, MM_HIMETRIC); /* 0.01mm */
    if (bfw.line == 0 && fromleft == 0) {
      if (side == 1) {
        StartPage(hDCPrinter);
      }
      page++;
      print_form(hWnd, hDCPrinter, &systemtime, side, page, filename);
    }

    /* calc rectangle of the main text body. */
    if (side == 1) {
      /* start printing out lines */
      bfw.start_pt.x = START_PTX1; bfw.start_pt.y = START_PTY;
      /* get heights of rectangle */
      bfw.end_pt.x = START_PTX1; bfw.end_pt.y = END_PTY;
      orikaeshi = ORIKAESHI1;
    }    else {
      /* start printing out lines */
      bfw.start_pt.x = START_PTX2; bfw.start_pt.y = START_PTY;
      /* get heights of rectangle */
      bfw.end_pt.x = START_PTX2; bfw.end_pt.y = END_PTY;;
      orikaeshi = ORIKAESHI2;
    }
    /* divide them by 66 makes a line height */
    bfw.rectangle_pt.x = bfw.end_pt.x - bfw.start_pt.x;
    bfw.rectangle_pt.y = bfw.end_pt.y - bfw.start_pt.y;

    /* must consider line length exceeds rectangle.x */

    /* get a letter */
    /* is sjis first?? */
    /* yes then get a second letter, no then store in text buffer */
    /* check CR/LF */
    bfw.c[0] = lpStr[currentpoint];
    /* last letter of lpVoid is NULL */
    if (bfw.c[0] != 0) {
      bfw.c[1] = lpStr[currentpoint + 1];
      bfw.c[2] = 0;
      if (lpStr[currentpoint + 1] != 0) {
        bfw.c[2] = lpStr[currentpoint + 2];
      } else {
        bfw.c[2] = 0;
        bfw.c[3] = 0;
      }
      if (lpStr[currentpoint + 2] != 0) {
        bfw.c[3] = lpStr[currentpoint + 3];
        bfw.c[4] = 0;
      }  else {
        bfw.c[3] = 0;
        bfw.c[4] = 0;
      }
    }    else {
      bfw.c[1] = 0;
    }

    if (bfw.c[0] == 0x0d && bfw.c[1] == 0x0a) {
      /* dos's return */
      /* print a line */
      buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
      fromleft = 0;
      bfw.line++;
      currentpoint += 2;
      /* clear text buffer */
#if 0
      lstrcpy(bf.text, "");
      lstrcpy(bf.croman, "");
      lstrcpy(bf.precroman, "");
#endif
      bfw.text[0] = '\0';
      bfw.croman[0] = '\0';
      bfw.precroman[0] = '\0';
    }  else if (bfw.c[0] == 0x0d || bfw.c[0] == 0x0a) {
      /* mac's or unix's return */
      /* print a line */
      buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
      lstrcpyW(bfw.text, L"");
      lstrcpyW(bfw.precroman, bfw.croman);
      fromleft = 0;
      bfw.line++;
      currentpoint++;
    } else if (bfw.c[0] == 9) {
      /* tab */
      /* if no printout the buffer */
      /* previous mode is ascii */
      /* get width of the text */
      buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
      lstrcpyW(bfw.text, L"");
      lstrcpyW(bfw.precroman, bfw.croman);
      bfw.kanjimode = 0;
      /* check length after each replace and 8 char */
      tab = 8 - (lstrlenW(bfw.croman) % 8);
      i = 0;
      bfw.c[0] = L' ';
      bfw.c[1] = L'\0';
      while (i < tab) {
        lstrcatW(bfw.text, bfw.c);
        fromleft++;
        lstrcatW(bfw.croman, L"M");
        /* check length */
        SelectObject(hDCPrinter, bfw.hFontMainRoman);
        GetTextExtentPoint32W(hDCPrinter, bfw.croman,
          lstrlenW(bfw.croman), &size);
        if (bfw.start_pt.x + size.cx > orikaeshi) {
          buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
          fromleft = 0;
          bfw.line++;
          /* clear text buffer */
#if 0
          lstrcpy(bf.text, "");
          lstrcpy(bf.croman, "");
          lstrcpy(bf.precroman, "");
#endif
          bfw.text[0] = L'\0';
          bfw.croman[0] = L'\0';
          bfw.precroman[0] = L'\0';
        }
        i++;
      }
      currentpoint++;
    } else if (is_utf8_zenkakuW(bfw.c[0], bfw.c[1], bfw.c[2], bfw.c[3])) {
      //wsprintfW(stdmsg, L"is_utf8_zenkakuW 000 c0 c1 c2 c3 = %X %X %X %X", bfw.c[0], bfw.c[1], bfw.c[2], bfw.c[3]);
      //MessageBoxW(hWnd, stdmsg, L"DEBUG", MB_OK);
      /* yes sjis 1st */
      /* previous state kanji?? */
      if (bfw.kanjimode) {
        /* if yes add the letter at the last */
      } else {
        /* if no printout the buffer */
        /* previous mode is ascii */
        /* get width of the text */
        buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
        lstrcpyW(bfw.text, L"");
        lstrcpyW(bfw.precroman, bfw.croman);
      }
      if (is_utf8_lead_2byteZenW(bfw.c[0])) {
        //MessageBoxA(hWnd, "is_utf8_lead_2byteZenW 000", "DEBUG", MB_OK);
        bfw.c[2] = L'\0';
        currentpoint += 2;
      } else if (is_utf8_lead_3byteZenW(bfw.c[0])) {
        //MessageBoxA(hWnd, "is_utf8_lead_3byteZenW 000", "DEBUG", MB_OK);
        bfw.c[3] = L'\0';
        currentpoint += 3;
      } else if (is_utf8_lead_4byteZenW(bfw.c[0])) {
        //MessageBoxA(hWnd, "is_utf8_lead_4byteZenW 000", "DEBUG", MB_OK);
        bfw.c[4] = L'\0';
        currentpoint += 4;
      }
      lstrcatW(bfw.text, bfw.c);
      fromleft += 2;
      //currentpoint += 2;
      lstrcatW(bfw.croman, L"MM");
      bfw.kanjimode = 1;
      /* check length */
      SelectObject(hDCPrinter, bfw.hFontMainRoman);
      GetTextExtentPoint32W(hDCPrinter, bfw.croman,
        lstrlenW(bfw.croman), &size);
      if (bfw.start_pt.x + size.cx > orikaeshi) {
        buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
        fromleft = 0;
        bfw.line++;
        /* clear text buffer */
#if 0
        lstrcpy(bf.text, "");
        lstrcpy(bf.croman, "");
        lstrcpy(bf.precroman, "");
#endif
        bfw.text[0] = '\0';
        bfw.croman[0] = '\0';
        bfw.precroman[0] = '\0';
      }
    } else if (is_utf8_hankaku_kanaW(bfw.c[0], bfw.c[1], bfw.c[2], bfw.c[3])) {
      //MessageBoxA(hWnd, "is_utf8_hankaku_kanaW 000", "DEBUG", MB_OK);
      /* yes hankaku */
      /* previous state kanji?? */
      if (bfw.kanjimode) {
        /* if yes add the letter at the last */
      }  else {
        /* if no printout the buffer */
        /* previous mode is ascii */
        /* get width of the text */
        buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
        lstrcpyW(bfw.text, L"");
        lstrcpyW(bfw.precroman, bfw.croman);
      }
      if (is_utf8_lead_2byteZenW(bfw.c[0])) {
        bfw.c[2] = L'\0';
        currentpoint += 2;
      } else if (is_utf8_lead_3byteZenW(bfw.c[0])) {
        bfw.c[3] = L'\0';
        currentpoint += 3;
      } else if (is_utf8_lead_4byteZenW(bfw.c[0])) {
        bfw.c[4] = L'\0';
        currentpoint += 4;
      }
      //bfw.c[1] = L'\0';
      lstrcatW(bfw.text, bfw.c);
      fromleft++;
      //currentpoint++;
      lstrcatW(bfw.croman, L"M");
      bfw.kanjimode = 1;
      /* check length */
      SelectObject(hDCPrinter, bfw.hFontMainRoman);
      GetTextExtentPoint32W(hDCPrinter, bfw.croman,
        lstrlenW(bfw.croman), &size);
      if (bfw.start_pt.x + size.cx > orikaeshi) {
        buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
        fromleft = 0;
        bfw.line++;
        /* clear text buffer */

        bfw.text[0] = L'\0';
        bfw.croman[0] = L'\0';
        bfw.precroman[0] = L'\0';
      }
    } else {
      /* ascii */
      /* previous state kanji?? */
      if (bfw.kanjimode) {
        /* if yes, flush text buffer */
        buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
        lstrcpyW(bfw.text, L"");
        lstrcpyW(bfw.precroman, bfw.croman);
      } else {
        /* if no add the letter at the last */
      }
      bfw.c[1] = L'\0';
      lstrcatW(bfw.text, bfw.c);
      fromleft++;
      currentpoint++;
      lstrcatW(bfw.croman, L"M");
      bfw.kanjimode = 0;
      /* check length */
      SelectObject(hDCPrinter, bfw.hFontMainRoman);
      GetTextExtentPoint32W(hDCPrinter, bfw.croman,
        lstrlenW(bfw.croman), &size);
      if (bfw.start_pt.x + size.cx > orikaeshi) {
        buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
        fromleft = 0;
        bfw.line++;
        /* clear text buffer */

        bfw.text[0] = '\0';
        bfw.croman[0] = '\0';
        bfw.precroman[0] = '\0';
      }
    }
    if (bfw.line >= 66) {
      if (side == 1) {
        side = 2;
      }
      else {
        EndPage(hDCPrinter);
        side = 1;
      }
      bfw.line = 0;
    }
  }

  /* need final flush */
  buffer_flush_utf8_W(hWnd, hDCPrinter, &bfw);
  lstrcpyW(bfw.text, L"");
  /* need endpage??? */
  EndPage(hDCPrinter);

  SelectObject(hDCPrinter, hFontOrg);
  DeleteObject(bfw.hFontMainRoman);
  DeleteObject(bfw.hFontMainKanji);

  SelectObject(hDCPrinter, hOldPen);
  DeleteObject(hPen);

  return TRUE;
}
/*----------------------------------------------------------------------*/
//BOOL ReadPrintFile(HWND hWnd, HDC hDCPrinter, char* filename)
BOOL ReadPrintFile(HWND hWnd, HDC hDCPrinter, WCHAR* filename)
{
    static LPVOID lpVoid;
    static HANDLE hHeap = NULL;
    HANDLE hFile;
    DWORD NumberOfBytesRead;
    static DWORD sizel, sizeh;
    WCHAR stdmsg[256];
    LPWSTR lpStr, lpStrb /*, lpStre */;
    MENUITEMINFOW menu_item_infow = { 0 };
    HMENU hMenu = NULL;

    if (!wcscmp(filename, L"")) {
        /* if null, return */
        wsprintf(stdmsg, L"The filename is invalid.");
        MessageBox(hWnd, stdmsg, L"Error!!", MB_OK);
        return FALSE;
    }

    lpStrb = filename;
    //if (NULL != (lpStr = strchr(lpStrb, '"'))) {
    if (NULL != (lpStr = wcschr(lpStrb, '"'))) {
        lpStrb = lpStr + 1;
        if (NULL != (lpStr = wcschr(lpStrb, '"'))) {
            *lpStr = L'\0';
        }
    }

#if 0
    lstrcpy(filename, "c:\\temp\\la00171.tmp");
#endif
    /* create file */
    if (INVALID_HANDLE_VALUE
        == (hFile = CreateFile(lpStrb, GENERIC_READ,
            FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL,
            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL))) {
        wsprintf(stdmsg, L"Function CreateFile(%s) has an error!! %d",
            lpStrb, GetLastError());
        MessageBox(hWnd, stdmsg, L"ERROR", MB_OK);
        return FALSE;
    }

    sizel = GetFileSize(hFile, &sizeh);
    if (sizeh != 0) {
        MessageBox(hWnd, L"File size is too huge!!",
            L"ERROR...", MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    hHeap = GetProcessHeap();
    if (hHeap != NULL) {
        /* last letter of lpVoid is NULL */
        lpVoid = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizel + 1);
        if (lpVoid != NULL) {
            /* read the file at a time */
            if (FALSE == ReadFile(hFile, lpVoid, sizel,
                &NumberOfBytesRead, NULL)) {
                MessageBox(hWnd, L"ReadFile Error...",
                    L"ERROR...", MB_OK | MB_ICONEXCLAMATION);
            }
        }        else {
            MessageBox(hWnd, L"HeapAlloc Error...",
                L"ERROR...", MB_OK | MB_ICONEXCLAMATION);
        }
    }    else {
        MessageBox(hWnd, L"GetProcessHeap Error...",
            L"ERROR...", MB_OK | MB_ICONEXCLAMATION);
    }
    CloseHandle(hFile);
    hMenu = GetMenu(hWnd);
    menu_item_infow.cbSize = sizeof(MENUITEMINFOW);
    menu_item_infow.fMask = MIIM_STATE;
    GetMenuItemInfoW(hMenu, ID_ENCODE_SHIFTJIS, FALSE, &menu_item_infow);
    if (menu_item_infow.fState & MFS_CHECKED) {
      // sjis
      if (hHeap != NULL) {
        /* process the buffer = output text */
        OutTextW(hWnd, lpVoid, hDCPrinter, lpStrb);
        HeapFree(hHeap, HEAP_NO_SERIALIZE, lpVoid);
      }
    }
    GetMenuItemInfoW(hMenu, ID_ENCODE_UTF, FALSE, &menu_item_infow);
    if (menu_item_infow.fState & MFS_CHECKED) {
      // sjis
      if (hHeap != NULL) {
        /* process the buffer = output text */
        OutTextUTF8W(hWnd, lpVoid, hDCPrinter, lpStrb);
        HeapFree(hHeap, HEAP_NO_SERIALIZE, lpVoid);
      }
    }
    return TRUE;
}
/*----------------------------------------------------------------------*/

//HWND _hDlgPrintAbort = NULL; /* handle of print abort dlg box */
//HINSTANCE _hDlgPrintAbort = NULL; /* handle of print abort dlg box */
#define GLOBALALLOC

LRESULT WINAPI PrintDialog(HWND hWnd, WCHAR* filename)
{
    //HANDLE hInstance;
    HINSTANCE hInstance;
    static PRINTDLG PD;
    static HDC hDCPrinter;
    static DOCINFO DocInfo;
    static WCHAR cDocName[] = L"Test Document";
    static HANDLE hLibrary_Old;
    static HMODULE hLibrary;
    WCHAR stdmsg[256];
    
    //static (int(__stdcall * (__cdecl*)(struct tagPDA*))(void))(*lpFuncPrintDlg)(LPPRINTDLG);
  
    _hDlgPrintAbort = NULL;

    //static FARPROC(*lpFuncPrintDlg)(LPPRINTDLG);
    static FARPROC(*lpFuncPrintDlg)(LPPRINTDLG);
    static DEVMODE DevMode;

#if 1
    /* def GLOBALALLOC */
    static HGLOBAL hDevMode;
    static DEVMODE* pDevMode;
#endif
#if 0
    DWORD ComErr;
    char debchar[256];
#endif

    //hInstance = (HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE);
    hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    ZeroMemory(&PD, sizeof(PRINTDLG));
    PD.lStructSize = sizeof(PRINTDLG);
    PD.hwndOwner = hWnd;
    PD.hDevNames = NULL;

#ifdef GLOBALALLOC
    hDevMode = GlobalAlloc(GHND, sizeof(DEVMODE));
    if (NULL != hDevMode) {
        PD.hDevMode = hDevMode;
        pDevMode = (DEVMODE*)GlobalLock(hDevMode);
        ZeroMemory(pDevMode, sizeof(DEVMODE));
        pDevMode->dmSize = sizeof(DEVMODE);
        pDevMode->dmDriverExtra = 0;
        pDevMode->dmFields = DM_ORIENTATION | DM_PAPERSIZE;
        pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
        pDevMode->dmPaperSize = DMPAPER_A4_TRANSVERSE;
        GlobalUnlock(hDevMode);
    }     else {
        MessageBox(NULL, L"global alloc error", L"DEBUG", MB_OK);
    }
#endif


#if 0
    PD.Flags = PD_RETURNDC | PD_PRINTSETUP;
#else
    PD.Flags = PD_USEDEVMODECOPIESANDCOLLATE
        | PD_RETURNDC
        | PD_HIDEPRINTTOFILE
        | PD_NOPAGENUMS
        | PD_NOSELECTION
        ;
#endif
    PD.nFromPage = 0xFFFF;
    PD.nToPage = 0xFFFF;
    PD.nMinPage = 1;
    PD.nMaxPage = 0xFFFF;
#if 0
    PD.nMinPage = 1;
    PD.nMaxPage = 1000;
#endif
    PD.nCopies = 1;

    hLibrary = LoadLibrary(L"comdlg32.dll");
    if (NULL == hLibrary) {
        MessageBox(hWnd, L"Could not load comdlg32.dll",
            L"LoadLibrary Error", MB_OK);
#ifdef GLOBALALLOC
        GlobalFree(hDevMode);
#endif
        return FALSE;
    }
    if (NULL !=
        (lpFuncPrintDlg = /* (void*) */
            //(int(__stdcall * (__cdecl*)(struct tagPDA*))(void)) GetProcAddress(hLibrary, "PrintDlg"))) {
            (FARPROC (*) (LPPRINTDLG))GetProcAddress(hLibrary, "PrintDlg"))) {
    }
    else if (NULL !=
        (lpFuncPrintDlg = /* (void*) */
            //(int(__stdcall * (__cdecl*)(struct tagPDA*))(void)) GetProcAddress(hLibrary, "PrintDlgA"))) {
            (FARPROC(*) (LPPRINTDLG))GetProcAddress(hLibrary, "PrintDlgA"))) {
    }
    else if (NULL !=
        (lpFuncPrintDlg = /* (void*) */
            //(int(__stdcall * (__cdecl*)(struct tagPDA*))(void)) GetProcAddress(hLibrary, "PrintDlgW"))) {
            (FARPROC(*)(LPPRINTDLG))GetProcAddress(hLibrary, "PrintDlgW"))) {
    }
    else {
        MessageBox(hWnd, L"Could not find any PrintDlg",
            L"ERROR", MB_OK);
        FreeLibrary(hLibrary);
#ifdef GLOBALALLOC
        GlobalFree(hDevMode);
#endif
        return FALSE;
    }
    /* show dialog box */

    //pDevMode = (DEVMODE*)GlobalLock(PD.hDevMode);
    //pDevMode->dmFields = DM_ORIENTATION;
    //pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
    if (FALSE == ResetDC(PD.hDC, pDevMode)) {

    }

    if (TRUE == (BOOL)(*lpFuncPrintDlg)(&PD)) {
        /* (int) ,(int (*__import near std func)(LPPRINTDLG)) */
        if (NULL == PD.hDevMode) {
            MessageBox(hWnd, L"PD.hDevMode is NULL.", L"DEBUG...", MB_OK);
        }        else {
            //wsprintf(stdmsg, L"dev orient= %ld", pDevMode->dmOrientation);
            //MessageBox(hWnd, stdmsg, L"DEBUG...", MB_OK);
            //pDevMode = (DEVMODE*)GlobalLock(PD.hDevMode);
            //pDevMode->dmFields = DM_ORIENTATION;
            //pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
            pDevMode->dmFields = DM_ORIENTATION | DM_PAPERSIZE;
            pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
            pDevMode->dmPaperSize = DMPAPER_A4_TRANSVERSE;

        }

#if 1
        if (FALSE == ResetDC(PD.hDC, pDevMode)) {
#if 0
            MessageBox(hWnd, "Fail in ResetDC", "DEBUG...", MB_OK);
#endif
        }
        //GlobalUnlock(PD.hDevMode);
#endif
#if 0
        DevMode.dmFields = DM_ORIENTATION;
#endif
#if 0
        DevMode.dmOrientation = DMORIENT_LANDSCAPE;
        if (FALSE == ResetDC(PD.hDC, &DevMode)) {
#if 0
            MessageBox(hWnd, "Fail in ResetDC", "DEBUG...", MB_OK);
#endif
        }
#endif
        hDCPrinter = PD.hDC; /* printer DC */

        // set up abort proceure section
        // disable the main window, dlg box active
        EnableWindow(hWnd, FALSE);
        // put the modeless dialog box on the screen
        //_hDlgPrintAbort = CreateDialog(hInstance,

        //BOOL WINAPI PrintStopDlg(HWND hDlg, UINT wMessage, WPARAM wParam,
        //    LPARAM lParam)

        // good
        //CreateDialog(hInstance,
        //        L"PrintStop", hWnd, /*(void*)*/ (int(__stdcall*)(void)) PrintStopDlg);
        //L"PrintStop", hWnd, /*(void*)*/ (DLGPROC) PrintStopDlg);

        //void CreateDialogA(
        //  [in, optional]  hInstance,
        //  [in]            lpName,
        //  [in, optional]  hWndParent,
        //  [in, optional]  lpDialogFunc
        //);

        //HWND CreateDialogParamA(
        //  [in, optional] HINSTANCE hInstance,
        //  [in]           LPCSTR    lpTemplateName,
        //  [in, optional] HWND      hWndParent,
        //  [in, optional] DLGPROC   lpDialogFunc,
        //  [in]           LPARAM    dwInitParam
        //);

        _hDlgPrintAbort = CreateDialogParamW(hInstance, L"PrintStop",
              hWnd, (DLGPROC)PrintStopDlg, 0);

        // set the global variable FALSE to start
        _bPrintAbort = FALSE;
        // make proc instance address for abort proc.
        // tell Windows to send messages to the
        // PrintAbort() function's message loop
        SetAbortProc(hDCPrinter, (ABORTPROC)PrintAbort);
        // init DOCINFO structure

        DocInfo.cbSize = sizeof(DOCINFO);
        DocInfo.lpszDocName = cDocName;
        DocInfo.lpszOutput = NULL;

        if (0 < StartDoc(hDCPrinter, &DocInfo)) {/* output to printer */
            /*
                  OutText(hWnd, hDCPrinter);
            */
            ReadPrintFile(hWnd, hDCPrinter, filename);
            EndDoc(hDCPrinter);// normally, this section would
#if 0
            MessageBox(hWnd, "StartDoc OK...", "DEBUG...", MB_OK);
#endif
        }        else {
            // procedure.
            // printing and check the abort
            // be a loop that cycles
            AbortDoc(hDCPrinter);
            MessageBox(hWnd, L"Could not create printer dc.",
                L"Warning", MB_OK);
        }

        if (NULL != _hDlgPrintAbort) {
          DestroyWindow(_hDlgPrintAbort);
          _hDlgPrintAbort = NULL;

        }

        EnableWindow(hWnd, TRUE);

        DeleteDC(hDCPrinter);

        if (hLibrary != NULL) {
            FreeLibrary(hLibrary);
        }
#ifdef GLOBALALLOC
        GlobalFree(hDevMode);
#endif
        return TRUE;
    }
    else {
        if (hLibrary != NULL) {
            FreeLibrary(hLibrary);
        }
#if 0
        hLibrary = LoadLibrary("comdlg32.dll");
        if ((int)hLibrary < 32) {
            MessageBox(hWnd, "Could not load comdlg32.dll",
                "LoadLibrary Error", MB_OK);
#ifdef GLOBALALLOC
            GlobalFree(hDevMode);
#endif
            return FALSE;
        }
        if (NULL !=
            (lpFuncPrintDlg = (void*)GetProcAddress(hLibrary, "CommDlgEx"))) {
        }
        else if (NULL !=
            (lpFuncPrintDlg = (void*)GetProcAddress(hLibrary, "PrintDlgA"))) {
        }
        else if (NULL !=
            (lpFuncPrintDlg = (void*)GetProcAddress(hLibrary, "PrintDlgW"))) {
        }
        else {
            MessageBox(hWnd, "Could not find any PrintDlg",
                "ERROR", MB_OK);
            FreeLibrary(hLibrary);
#ifdef GLOBALALLOC
            GlobalFree(hDevMode);
#endif
            return FALSE;
        }

        ComErr = CommDlgExtendedError();
        wsprintf(debchar, "ComErr = 0x%x", ComErr);
        MessageBox(hWnd, debchar, "DEBUG...", MB_OK);
#endif
        // user cancelled printing
#ifdef GLOBALALLOC
        GlobalFree(hDevMode);
#endif
        return FALSE;
    }
#ifdef GLOBALALLOC
    GlobalFree(hDevMode);
#endif
}
/*----------------------------------------------------------------------*/
#if 0
void OutputStuff(HDC hDCPrinter, HANDLE hInstance)
{
    static LOGFONT LogFont;
    static HFONT hFont, hOldFont;

    char cBuf[128];
    DWORD dwTextExtent;

    /* restore font information */
    RestoreFontInfomationFromFile(&LogFont);
    hFont = CreateFontIndirect(&LogFont);
    hOldFont = SelectObject(hDCPrinter, hFont);

    TextOut(hDCPrinter, 0, 0,
        "This should show up on your printer.", 36);

    hFont = SelectObject(hDCPrinter, hOldFont);
    DeleteObject(hFont);

#if 0
    LoadString(hInstance, S_OUTTEXT1, cBuf, sizeof(cBuf));
    // output first line of text at 0,0
    TextOut(hDC, 0, 0, cBuf, lstrlen(cBuf));
    dwTextExtent = GetTextExtent(hDC, cBuf, lstrlen(cBuf));
    LoadString(hInstance, S_OUTTEXT2, cBuf, sizeof(cBuf));
    // output second line of text 2 mm below first
    TextOut(hDC, 0, -1 * HIWORD(dwTextExtent) - 20,
        cBuf, lstrlen(cBuf));
#endif
}
#endif
/*----------------------------------------------------------------------*/
void CharToNulls(LPWSTR lpString, char c)
{
    int i, nLong;

    nLong = lstrlen(lpString) + 1;
    for (i = 0; i < nLong; i++) {
        if (*lpString == c) {
            *lpString = 0;
        }
        lpString++;
    }
}
/*----------------------------------------------------------------------*/
int open_file(HWND hWnd, WCHAR* filename)
{
    //HANDLE hLibrary;
    HMODULE hLibrary;
    static OPENFILENAME OFN;
    WCHAR message[256];
    static FARPROC(*fpGetFileTitle)(LPCTSTR, LPTSTR, WORD);
    static FARPROC(*fpGetOpenFileName)(LPOPENFILENAME);
    WCHAR cFilter[] =
        L"All Files (*.*)|*.*|Text Files (*.txt)|*.txt|"
        L"Mail Files (*.ms*)|*.ms*|Sjis Files (*.sj*)|*.sj*|";
    static WCHAR cStartFilter[40] = L"",
        cFileName[256] = L"",
        cShortFileName[16] = L"",
        cDefaultExt[] = L"msg";
    BOOL bGotFile;

    _hDlgPrintAbort = NULL;

    hLibrary = LoadLibrary(L"comdlg32.dll");
    if (NULL == hLibrary) {
        wsprintf(message, L"open file hLibrary = %d", hLibrary);
        MessageBox(hWnd, message, L"Debug", MB_OK);
        return FALSE;
    }

    if (NULL == (fpGetOpenFileName
        = /* (void*) */
        
        //(int(__stdcall * (__cdecl*)(struct tagOFNA*))(void))
        (FARPROC(*)(LPOPENFILENAME))
        //GetProcAddress(hLibrary, L"GetOpenFileNameA"))) {
        //FARPROC GetProcAddress(
        //    [in] HMODULE hModule,
        //    [in] LPCSTR  lpProcName
        //);

        GetProcAddress(hLibrary, "GetOpenFileNameW"))) {
        MessageBox(hWnd, L"Can't find Function GetOpenFileNameW", L"Debug", MB_OK);
        FreeLibrary(hLibrary);
        return FALSE;
    }

    if (NULL == (fpGetFileTitle
        = /* (void*) */
        //(int(__stdcall * (__cdecl*)(const char*, char*, unsigned short))(void))
        //GetProcAddress(hLibrary, "GetFileTitleA"))) {
        (FARPROC(*)(LPCTSTR, LPTSTR, WORD))
        GetProcAddress(hLibrary, "GetFileTitleW"))) {
        MessageBox(hWnd, L"Can't find Function GetFileTitleW", L"DEBUG", MB_OK);
        FreeLibrary(hLibrary);
        return FALSE;
    }

    CharToNulls(cFilter, L'|');
    CharToNulls(cStartFilter, L'|');
    OFN.lStructSize = sizeof(OPENFILENAME);
    OFN.hwndOwner = hWnd;
    OFN.lpstrFilter = cFilter;
    OFN.lpstrCustomFilter = cStartFilter;
    OFN.nMaxCustFilter = 40;
    OFN.nFilterIndex = 1;
    OFN.lpstrFile = cFileName;
    OFN.nMaxFile = 128;
    OFN.lpstrFileTitle = cShortFileName;
    OFN.lpstrDefExt = cDefaultExt;
    OFN.nMaxFileTitle = 16;
    OFN.lpstrInitialDir = NULL;
    OFN.lpstrTitle = APPNAME" File Open";
    OFN.Flags = OFN_HIDEREADONLY | OFN_NOVALIDATE
        | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    if (FALSE == (bGotFile = (int)(*fpGetOpenFileName)(&OFN))) {
        filename[0] = '\0';
        FreeLibrary(hLibrary);
        return FALSE;
    }
    else {
        /* succeeded to get file name */
        lstrcpy(filename, OFN.lpstrFile);
    }
    FreeLibrary(hLibrary);
    /* success */
    return TRUE;
}
/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
/** AboutDlgProc - message handler for the About dialog box **/
BOOL WINAPI AboutDlgProc(HWND hDlg, UINT message,
//DLGPROC   AboutDlgProc(HWND hDlg, UINT message,
        WPARAM wParam, LPARAM lParam)
{
    WCHAR debbuf[33];
#if 0
    unsigned long os_version;
#endif

    switch (message) {
    case WM_INITDIALOG:
#if 0
        GetPrivateProfileString((LPSTR)INI_FILE_ENTRY, (LPSTR)"os_version",
            (LPSTR)"3", (LPSTR)debbuf, 32, (LPSTR)INI_FILE_NAME);
        os_version = strtoul(debbuf, NULL, 16);
#endif
        //wsprintf(debbuf, L"Windows Ver. %ld", LOBYTE(LOWORD(GetVersion())));
        wsprintf(debbuf, L"a2p64 Ver. 1.0");

        SendDlgItemMessage(hDlg, 108, WM_SETTEXT, (WPARAM)0, (LPARAM)debbuf);
        return(TRUE);
        /** when user clicks the only button (OK), then close the about box */
    case WM_COMMAND:
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, TRUE);
            return(TRUE);
        }
    }
    return(FALSE);
}
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/** WndProc - primary message handler **/
//long FAR PASCAL WndProc(HWND hWnd, UINT message,
//    WPARAM wParam, LPARAM lParam)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

{
    /* need */
    RECT rect, rect_window, rect_desktop, rect_menu_r, rect_menu_l;
    HMENU hMenu = NULL;
    HCURSOR hCursor;
    LPWSTR lpStr, lpClip;
    WCHAR appname[80] = APPNAME" ";
    //static HANDLE hInstance;
    static HINSTANCE hInstance;
    static HWND hWndDesktop;
    static WCHAR stdmsg[256];
    static WCHAR filename[256];
    static WCHAR temppath[256];
    WCHAR command_line[256];
    static WCHAR tmpbuf[MYCHARBUFLEN];
#if 0
    DWORD dwStyle;
#endif
    DWORD dwNumberOfBytesWritten;
    UINT uParam;
    HANDLE hMem, hClipMem;
    HANDLE hFile;
    HKEY hkResult;
    DWORD dwDisposition;
    DWORD dwKeyValue, dwType, cbData;
    long width, hight;
    static long max_hight = 0;
    /* This variable must be a static */
    static BOOL bIsQuitMode = FALSE;
    static BOOL bLbutton = FALSE;

    /* start program */
    switch (message) {
    case WM_CREATE:
      hMenu = GetMenu(hWnd);
      CheckMenuItem(hMenu, ID_ENCODE_SHIFTJIS, MF_CHECKED);
      lstrcpy(command_line, GetCommandLine());
        ParseCommandLine(command_line, filename, &bIsQuitMode);
        if (filename[0] != '\0') {
            PostMessage(hWnd, WM_COMMAND, MAKELONG(IDM_PRINT, 0), 0L);
        }
        break;
    case WM_DROPFILES:
        filename[0] = '\0'; /* initialize */
        if (1 == DragQueryFile((HDROP)wParam, 0xFFFFFFFF,
            tmpbuf, 256)) {
            DragQueryFile((HDROP)wParam, 0, filename, 256);
            if (filename[0] != '\0') {
                PrintDialog(hWnd, filename);
            }
        }        else {
            MessageBox(hWnd, L"Only one file is acceptable...",
                L"Ouch!!", MB_OK | MB_ICONASTERISK);
        }
        DragFinish((HDROP)wParam);
        if (bIsQuitMode == TRUE) {
            PostMessage(hWnd, WM_CLOSE, 0, 0L);
        }
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            /* add copy param from WinMain */
        case ID_ENCODE_SHIFTJIS:
          hMenu = GetMenu(hWnd);
          CheckMenuItem(hMenu, ID_ENCODE_UTF, MF_UNCHECKED);
          CheckMenuItem(hMenu, ID_ENCODE_SHIFTJIS, MF_CHECKED);
          break;
        case ID_ENCODE_UTF:
          hMenu = GetMenu(hWnd);
          CheckMenuItem(hMenu, ID_ENCODE_SHIFTJIS, MF_UNCHECKED);
          CheckMenuItem(hMenu, ID_ENCODE_UTF, MF_CHECKED);
          //MessageBoxA(NULL, "utf8", "debug", MB_OK);
          break;
        case IDM_OPEN_PRINT:
            open_file(hWnd, filename);
        case IDM_PRINT:
            if (lParam != 0) {
                lstrcpy(filename, (LPCWSTR)lParam);
            }
            if (filename[0] != '\0') {
                PrintDialog(hWnd, filename);
            }
            if (bIsQuitMode == TRUE) {
                PostMessage(hWnd, WM_CLOSE, 0, 0L);
            }
            break;
        case IDM_Quit:
            SendMessage(hWnd, WM_CLOSE, 0, 0L);
            return 0;
        case IDM_PASTE:
            /* print clip board */
            if (OpenClipboard(hWnd)) {
                /* get a memory handle */
                /* is the format text? */
                hClipMem = GetClipboardData(CF_TEXT);
                if (hClipMem == NULL) {
                    MessageBox(hWnd, L"Clipboard doesn't have CF_TEXT data format.",
                        L"Error...", MB_OK);
                    CloseClipboard();
                }
                else {
                    /* yes, then print */
                    hMem = GlobalAlloc(GHND, GlobalSize(hClipMem));
                    lpStr = (LPWSTR)GlobalLock(hMem);
                    lpClip = (LPWSTR)GlobalLock(hClipMem);
                    lstrcpy(lpStr, lpClip);
                    GlobalUnlock(hMem);
                    GlobalUnlock(hClipMem);
                    CloseClipboard();
                    GetTempPath(sizeof(temppath), temppath);
                    GetTempFileName(temppath, L"a2p", 0, filename);
                    if (INVALID_HANDLE_VALUE
                        == (hFile = CreateFile(filename, GENERIC_WRITE,
                            FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL))) {
                        wsprintf(stdmsg, L"Function CreateFile(%s) faied!!", filename);
                        MessageBox(hWnd, stdmsg, L"ERROR", MB_OK);
                    }
                    else {
                        WriteFile(hFile, lpStr, lstrlen(lpStr),
                            &dwNumberOfBytesWritten, NULL);
                        CloseHandle(hFile);
                        /* printout here */
                        PrintDialog(hWnd, filename);
                        DeleteFile(filename);
                    }
                }
            }
            if (bIsQuitMode == TRUE) {
                PostMessage(hWnd, WM_CLOSE, 0, 0L);
            }
            break;
        case IDM_Help:
            hCursor = SetCursor(LoadCursor(NULL, IDC_ICON));
            SetCursor(hCursor);
            return TRUE;
        case IDM_HelpDoc:
            hCursor = SetCursor(LoadCursor(NULL, IDC_ICON));

            if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
                L"Software\\WhiteRock\\a2p", 0, NULL,
                REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_EXECUTE,
                NULL, &hkResult, &dwDisposition)) {
            }
            if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"myself", NULL,
                &dwType, (LPBYTE)NULL, &cbData)) {
            }
            if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"myself", NULL,
                &dwType, (LPBYTE)tmpbuf, &cbData)) {
            }
            RegCloseKey(hkResult);
#if 0
            GetCurrentDirectory(MYCHARBUFLEN, tmpbuf);
#endif
            lstrcpy(filename, L"write.exe ");
            //while (NULL != (lpStr = strchr(tmpbuf, '"'))) {
                while (NULL != (lpStr = wcschr(tmpbuf, L'"'))) {
                    /* replace a double quote to a space */
                *lpStr = ' ';
            }
            //lpStr = strrchr(tmpbuf, L'\\');
            lpStr = wcschr(tmpbuf, L'\\');
            if (lpStr != NULL) {
                *lpStr = L'\0';
            }
#if 0
            MessageBox(hWnd, tmpbuf, "debug", MB_OK);
#endif
            lstrcat(tmpbuf, APP_HELP_DOC);
            lstrcat(filename, tmpbuf);
            if (WinExec((LPSTR)filename, SW_SHOWNORMAL) < 32) {
                wsprintf(stdmsg, L"Can not run write.exe");
                MessageBox(hWnd, stdmsg, L"Ouch!!", MB_OK | MB_ICONASTERISK);
                SetCursor(hCursor);
                return FALSE;
            }
            SetCursor(hCursor);
            return TRUE;
        case IDM_HelpText:
            hCursor = SetCursor(LoadCursor(NULL, IDC_ICON));
            if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER,
                L"Software\\WhiteRock\\a2p", 0, NULL,
                REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_EXECUTE,
                NULL, &hkResult, &dwDisposition)) {
            }
            if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"myself", NULL,
                &dwType, (LPBYTE)NULL, &cbData)) {
            }
            if (ERROR_SUCCESS == RegQueryValueEx(hkResult, L"myself", NULL,
                &dwType, (LPBYTE)tmpbuf, &cbData)) {
            }
            RegCloseKey(hkResult);

            lstrcpy(filename, L"write.exe ");
            //while (NULL != (lpStr = strchr(tmpbuf, L'"'))) {
                while (NULL != (lpStr = wcschr(tmpbuf, L'"'))) {
                    /* replace a double quote to a space */
                *lpStr = ' ';
            }
            //lpStr = strrchr(tmpbuf, L'\\');
            lpStr = wcschr(tmpbuf, L'\\');
            if (lpStr != NULL) {
                *lpStr = '\0';
            }

            lstrcpy(filename, L"write.exe ");
            lstrcat(tmpbuf, APP_HELP_TEXT);
            lstrcat(filename, tmpbuf);
            if (WinExec((LPSTR)filename, SW_SHOWNORMAL) < 32) {
                wsprintf(stdmsg, L"Can not run write.exe");
                MessageBox(hWnd, stdmsg, L"Ouch!!", MB_OK | MB_ICONASTERISK);
                SetCursor(hCursor);
                return FALSE;
            }
            SetCursor(hCursor);
            return TRUE;
        case IDM_About:
            //DialogBox(hInstance, L"ABOUT", hWnd, /*(void*)*/(int(__stdcall*)(void)) AboutDlgProc);
            DialogBox(hInstance, L"ABOUT", hWnd, (DLGPROC)AboutDlgProc);
            return 0;
        }
        break;
#if 1
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_ACTIVE) {
            SetForegroundWindow(GetNextWindow(hWnd, GW_HWNDNEXT));
        }
        break;
#endif
    case WM_NCPAINT:
        /* if 0 for print test */
        hMenu = GetMenu(hWnd);
        GetWindowRect(hWnd, &rect_window);
        GetClientRect(hWnd, &rect);
        GetMenuItemRect(hWnd, hMenu, 0, &rect_menu_l);
        GetMenuItemRect(hWnd, hMenu, GetMenuItemCount(hMenu) - 1, &rect_menu_r);
        if (wParam == SIZE_MAXIMIZED) {
            uParam = SWP_NOZORDER | SWP_SHOWWINDOW;
            hWndDesktop = GetDesktopWindow();
            GetWindowRect(hWndDesktop, &rect_desktop);
            width = rect_desktop.right - rect_desktop.left;
        }
        else {
            uParam = SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW;
            width = rect_window.right - rect_window.left;
        }
        if (rect.bottom == 0) {
            if (max_hight < rect_menu_r.bottom - rect_window.top
                /* add boarder thickness */
                + rect_menu_l.left - rect_window.left) {
                max_hight = rect_menu_r.bottom - rect_window.top
                    + rect_menu_l.left - rect_window.left;
            }
        }
        if (rect.bottom > 0) {
            hight = rect_menu_r.bottom - rect_window.top - rect.bottom
                + rect_menu_l.left - rect_window.left;
            max_hight = hight;
        }
        else {
            hight = max_hight;
        }
        SetWindowPos(hWnd, HWND_TOPMOST,
            0, 0, width, hight, uParam);
#if 0
        if (wParam == SIZE_MAXIMIZED) {
            return 0;
        }
#endif
        break;
#if 0
    case WM_LBUTTONDOWN:
        bLbutton = TRUE;
        break;
    case WM_LBUTTONUP:
        bLbutton = FALSE;
        break;
#endif
#if 0
    case WM_MOVING:
        uParam = GetWindowLong(hWnd, GWL_STYLE);
        GetWindowRect(hWnd, &rect_window);
        GetClientRect(hWnd, &rect);
        if (uParam & WS_MAXIMIZE) {
            uParam = SWP_NOZORDER | SWP_SHOWWINDOW;
            hWndDesktop = GetDesktopWindow();
            GetWindowRect(hWndDesktop, &rect_desktop);
            width = rect_desktop.right - rect_desktop.left;
#if 1
            SetWindowPos(hWnd, HWND_TOPMOST,
                0, 0,
                width,
                rect_window.bottom - rect_window.top
                - (rect.bottom - rect.top > 0 ? rect.bottom - rect.top : 0),
                uParam);
#endif
            return TRUE;
        }
        break;
#endif
    case WM_MOVE:
        uParam = GetWindowLong(hWnd, GWL_STYLE);
        GetWindowRect(hWnd, &rect_window);
        GetClientRect(hWnd, &rect);
        if (uParam & WS_MAXIMIZE) {
            uParam = SWP_NOZORDER | SWP_SHOWWINDOW;
            hWndDesktop = GetDesktopWindow();
            GetWindowRect(hWndDesktop, &rect_desktop);
            width = rect_desktop.right - rect_desktop.left;
#if 1
            SetWindowPos(hWnd, HWND_TOPMOST,
                0, 0,
                width,
                rect_window.bottom - rect_window.top
                - (rect.bottom - rect.top > 0 ? rect.bottom - rect.top : 0),
                uParam);
#endif
            return 0;
        }
        RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\WhiteRock\\a2p",
            NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
            &hkResult, &dwDisposition);
        dwKeyValue = rect_window.left;
        RegSetValueEx(hkResult, L"X", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        dwKeyValue = rect_window.top;
        RegSetValueEx(hkResult, L"Y", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        dwKeyValue = rect_window.right - rect_window.left;
        RegSetValueEx(hkResult, L"nWidth", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        dwKeyValue = rect_window.bottom - rect_window.top;
        RegSetValueEx(hkResult, L"nHeight", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        RegCloseKey(hkResult);
        break;
    case WM_SIZE:
        GetWindowRect(hWnd, &rect_window);
        RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\WhiteRock\\a2p",
            0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
            &hkResult, &dwDisposition);
        dwKeyValue = rect_window.left;
        RegSetValueEx(hkResult, L"X", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        dwKeyValue = rect_window.top;
        RegSetValueEx(hkResult, L"Y", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        dwKeyValue = rect_window.right - rect_window.left;
        RegSetValueEx(hkResult, L"nWidth", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        dwKeyValue = rect_window.bottom - rect_window.top;
        RegSetValueEx(hkResult, L"nHeight", NULL, REG_DWORD,
            (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
        RegCloseKey(hkResult);
        break;
    case WM_DESTROY:
        /* if you finish this program minimized, */
        /* next time the position must be changed */
        if (bIsQuitMode == FALSE) {
            RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\WhiteRock\\a2p",
                NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                &hkResult, &dwDisposition);
            dwKeyValue = 0;
            RegSetValueEx(hkResult, L"hWnd", NULL, REG_QWORD,
                (CONST BYTE*) & dwKeyValue, sizeof(dwKeyValue));
            RegCloseKey(hkResult);
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
/*----------------------------------------------------------------------*/

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About_No_need(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
