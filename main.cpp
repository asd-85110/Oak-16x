#include <windows.h>

#include <string>

#include <fstream>

#include <vector>
#include <commdlg.h>
#include <uxtheme.h>
#include <shlobj.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "shell32.lib")

// 全局变量
HWND hEdit;
HINSTANCE hInst;
HMENU hMenu;
bool hexMode = true;

std::vector<BYTE> fileData;

std::string currentFilePath;

// 函数声明

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void SwitchDisplayMode();

void UpdateDisplay();

void LoadFile(const char* filePath);

void SaveFile(const char* filePath);

void RegisterFileAssociation();

std::string BytesToHex(const std::vector<BYTE>& data);

std::vector<BYTE> HexToBytes(const std::string& hex);

std::vector<BYTE> StringToBytes(const std::string& str);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    FreeConsole();

    // 检查命令行参数
    if (strlen(lpCmdLine) > 0) {
        currentFilePath = lpCmdLine;
        // 去除引号
        if (currentFilePath.front() == '"' && currentFilePath.back() == '"') {
            currentFilePath = currentFilePath.substr(1, currentFilePath.length() - 2);
        }
    }

    // 注册窗口类
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "HexEditorClass";
    wcex.hIconSm = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

    if (!RegisterClassEx(&wcex)) {
        return 1;
    }

    // 创建主窗口
    HWND hwnd = CreateWindowExA(0, "HexEditorClass", "Oak-16 Hex Editor v2.1.1", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
        800, 600, NULL, NULL, hInstance, NULL);

    SetWindowTheme(hwnd, L"Explorer", NULL);

    if (!hwnd) {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 如果有命令行文件，自动加载
    if (!currentFilePath.empty()) {
        LoadFile(currentFilePath.c_str());
    }

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
HMENU CreateMainMenu() {
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreatePopupMenu();
    HMENU hEditMenu = CreatePopupMenu();
    // 文件菜单
    AppendMenuA(hFileMenu, MF_STRING, 300, "新建(&N)");
    AppendMenuA(hFileMenu, MF_STRING, 301, "打开(&O)");
    AppendMenuA(hFileMenu, MF_STRING, 302, "保存(&S)");
    AppendMenuA(hFileMenu, MF_STRING, 303, "另存为(&A)");
    AppendMenuA(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hFileMenu, MF_STRING, 304, "设置文件关联");
    AppendMenuA(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hFileMenu, MF_STRING, 305, "退出(&X)");

    // 编辑菜单
    AppendMenuA(hEditMenu, MF_STRING, 400, "复制(&C)");
    AppendMenuA(hEditMenu, MF_STRING, 401, "剪切(&X)");
    AppendMenuA(hEditMenu, MF_STRING, 402, "粘贴(&V)");
    AppendMenuA(hEditMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuA(hEditMenu, MF_STRING, 403, "十六进制模式");
    AppendMenuA(hEditMenu, MF_STRING, 404, "文本模式");

    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "文件(&F)");
    AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, "编辑(&E)");

    return hMenu;
}
// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
        // 创建菜单
            hMenu = CreateMainMenu();
            SetMenu(hwnd, hMenu);
                    // 创建编辑框
        hEdit = CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "EDIT",
            "",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | 
            ES_AUTOHSCROLL | WS_VSCROLL | ES_AUTOVSCROLL,
            10, 10, 780, 500,
            hwnd, (HMENU)100, hInst, NULL);

        // 设置字体为"黑体 常规"
        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, "黑体");
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;
    }

    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        
        SetWindowPos(hEdit, NULL, 10, 10, width - 20, height - 20, SWP_NOZORDER);
        break;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
            case 300: // 新建
                fileData.clear();
                currentFilePath.clear();
                SetWindowTextA(hEdit, "");
                SetWindowTextA(hwnd, "Oak-16 Hex Editor v2.1.1 - 新文件");
                break;

            case 301: // 打开
                {
                    OPENFILENAMEA ofn;
                    char szFile[260] = {0};
                    
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
                    if (GetOpenFileNameA(&ofn)) {
                        currentFilePath = szFile;
                        LoadFile(currentFilePath.c_str());
                        char title[300];
                        sprintf(title, "Oak-16 Hex Editor v2.1.1 - %s", szFile);
                        SetWindowTextA(hwnd, title);
                    }
                }
                break;

            case 302: // 保存
                if (!currentFilePath.empty()) {
                    SaveFile(currentFilePath.c_str());
                } else {
                    // 调用另存为
                    SendMessage(hwnd, WM_COMMAND, 303, 0);
                }
                break;

            case 303: // 另存为
                {
                    OPENFILENAMEA ofn;
                    char szFile[260] = {0};
                    
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = "All Files\0*.*\0";
                    ofn.nFilterIndex = 1;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
                    
                    if (GetSaveFileNameA(&ofn)) {
                        currentFilePath = szFile;
                        SaveFile(currentFilePath.c_str());
                        char title[300];
                        sprintf(title, "Oak-16 Hex Editor v2.1.1 - %s", szFile);
                        SetWindowTextA(hwnd, title);
                    }
                }
                break;

                case 304: // 设置文件关联
                    RegisterFileAssociation();
                    break;

                case 305: // 退出
                    PostQuitMessage(0);
                    break;

                case 400: // 复制
                    SendMessage(hEdit, WM_COPY, 0, 0);
                    break;

                case 401: // 剪切
                    SendMessage(hEdit, WM_CUT, 0, 0);
                    break;

                case 402: // 粘贴
                    SendMessage(hEdit, WM_PASTE, 0, 0);
                    break;

                case 403: // 十六进制模式
                    if (!hexMode) {
                        SwitchDisplayMode();
                    }
                    break;

                case 404: // 文本模式
                    if (hexMode) {
                        SwitchDisplayMode();
                    }
                    break;
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
// 切换显示模式
void SwitchDisplayMode() {
    // 获取当前编辑框内容
    int textLen = GetWindowTextLengthA(hEdit);
    if (textLen > 0) {
        char* buffer = new char[textLen + 1];
        GetWindowTextA(hEdit, buffer, textLen + 1);
    if (hexMode) {
        // 从十六进制模式切换到文本模式
        fileData = HexToBytes(buffer);
    } else {
        // 从文本模式切换到十六进制模式
        fileData = StringToBytes(buffer);
    }
    delete[] buffer;
    }

    hexMode = !hexMode;
    UpdateDisplay();

    // 更新菜单项状态
    HMENU hEditMenu = GetSubMenu(hMenu, 1);
    if (hexMode) {
        CheckMenuItem(hEditMenu, 403, MF_CHECKED);
        CheckMenuItem(hEditMenu, 404, MF_UNCHECKED);
    } else {
        CheckMenuItem(hEditMenu, 403, MF_UNCHECKED);
        CheckMenuItem(hEditMenu, 404, MF_CHECKED);
    }
}

// 更新显示
void UpdateDisplay() {
    if (hexMode) {
        std::string hexStr = BytesToHex(fileData);
        SetWindowTextA(hEdit, hexStr.c_str());
    } else {
        std::string textStr(fileData.begin(), fileData.end());
        SetWindowTextA(hEdit, textStr.c_str());
    }
}

// 加载文件
void LoadFile(const char* filePath) {
    HANDLE hFile = CreateFileA(filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD fileSize = GetFileSize(hFile, NULL);
        if (fileSize != INVALID_FILE_SIZE && fileSize < 10000000) { // 10MB限制
            fileData.resize(fileSize);
            DWORD bytesRead;
            if (ReadFile(hFile, fileData.data(), fileSize, &bytesRead, NULL)) {
                UpdateDisplay();
            }
        }
        CloseHandle(hFile);
    }
}

// 保存文件
void SaveFile(const char* filePath) {
    // 获取当前编辑框内容
    int textLen = GetWindowTextLengthA(hEdit);
    if (textLen > 0) {
        char* buffer = new char[textLen + 1];
        GetWindowTextA(hEdit, buffer, textLen + 1);
        std::vector<BYTE> saveData;
        if (hexMode) {
            saveData = HexToBytes(buffer);
        } else {
            saveData = StringToBytes(buffer);
        }
        
        HANDLE hFile = CreateFileA(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD bytesWritten;
            WriteFile(hFile, saveData.data(), saveData.size(), &bytesWritten, NULL);
            CloseHandle(hFile);
        }
        delete[] buffer;
    }
}
// 注册文件关联
void RegisterFileAssociation() {
    HKEY hKey;
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    // 注册多个文件扩展名
    const char* extensions[] = {".bin", ".a", ".lib", ".dat", ".hex"};

    for (int i = 0; i < 5; i++) {
        char keyName[256];
        sprintf(keyName, "%s", extensions[i]);
        
        if (RegCreateKeyExA(HKEY_CLASSES_ROOT, keyName, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)"Oak16HexEditor", strlen("Oak16HexEditor"));
            RegCloseKey(hKey);
        }
    }

    // 注册程序关联
    if (RegCreateKeyExA(HKEY_CLASSES_ROOT, "Oak16HexEditor", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)"Oak-16 Hex Editor Document", strlen("Oak-16 Hex Editor Document"));
        RegCloseKey(hKey);
    }

    // 注册打开命令
    if (RegCreateKeyExA(HKEY_CLASSES_ROOT, "Oak16HexEditor\\shell\\open\\command", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        char command[512];
        sprintf(command, "\"%s\" \"%%1\"", exePath);
        RegSetValueExA(hKey, NULL, 0, REG_SZ, (BYTE*)command, strlen(command));
        RegCloseKey(hKey);
    }

    // 通知系统更新
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

    MessageBoxA(NULL, "文件关联设置成功！现在可以用本程序直接打开 .bin, .a, .lib, .dat, .hex 文件。", "成功", MB_OK | MB_ICONINFORMATION);
}

// 字节数组转十六进制字符串
std::string BytesToHex(const std::vector<BYTE>& data) {
    std::string result;
    char hex[4];
    for (size_t i = 0; i < data.size(); i++) {
        if (i > 0 && (i % 20) == 0) {
            result += "\r\n";
        }
        sprintf(hex, "%02X ", data[i]);
        result += hex;
    }
    return result;
}

// 十六进制字符串转字节数组
std::vector<BYTE> HexToBytes(const std::string& hex) {
    std::vector<BYTE> bytes;
    std::string cleanHex;
    // 清理字符串，只保留十六进制字符
    for (char c : hex) {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
            cleanHex += c;
        }
    }

    // 每两个字符转换一个字节
    for (size_t i = 0; i < cleanHex.length(); i += 2) {
        if (i + 1 < cleanHex.length()) {
            std::string byteString = cleanHex.substr(i, 2);
            BYTE byte = (BYTE)strtol(byteString.c_str(), NULL, 16);
            bytes.push_back(byte);
        }
    }
    return bytes;
}

// 字符串转字节数组
std::vector<BYTE> StringToBytes(const std::string& str) {
    return std::vector<BYTE>(str.begin(), str.end());
}