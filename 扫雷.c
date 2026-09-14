#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define CELL 30      // 每个格子 30x30 像素
#define MAXN 50
#define MARGIN 15
#define TOPBAR 45    // 顶部信息栏高度

int mine[MAXN][MAXN];       // -1是雷
int revealed[MAXN][MAXN];   // 0未翻，1已翻
int rows, cols, landmine;
int gameOver = 0, win = 0;
int openedCount = 0;

void putlandmine(int r, int c, int (*p)[MAXN], int lm)
{
    srand((unsigned)time(NULL));
    for (int i = 0; i < lm; ) {
        int rr = rand() % r;
        int cc = rand() % c;
        if (p[rr][cc] == 0) {
            p[rr][cc] = -1;
            i++;
        }
    }
}

void count(int r, int c, int (*p)[MAXN])
{
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++) {
            if (p[i][j] == -1) continue;
            int cnt = 0;
            for (int a = i - 1; a <= i + 1; a++)
                for (int b = j - 1; b <= j + 1; b++)
                    if (a >= 0 && a < r && b >= 0 && b < c && p[a][b] == -1)
                        cnt++;
            p[i][j] = cnt;
        }
}

// 翻开空格子时自动连锁翻开周围的空白区
void floodReveal(int r, int c)
{
    if (r < 0 || r >= rows || c < 0 || c >= cols) return;
    if (revealed[r][c]) return;
    if (mine[r][c] == -1) return;
    revealed[r][c] = 1;
    openedCount++;
    if (mine[r][c] == 0) {
        for (int a = r - 1; a <= r + 1; a++)
            for (int b = c - 1; b <= c + 1; b++)
                floodReveal(a, b);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HFONT hFont = CreateFontA(20, 0, 0, 0, FW_BOLD, 0, 0, 0,
            DEFAULT_CHARSET, 0, 0, 0, 0, "Consolas");
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                int x = MARGIN + j * CELL;
                int y = MARGIN + TOPBAR + i * CELL;
                RECT rc = { x, y, x + CELL, y + CELL };

                COLORREF bg;
                if (revealed[i][j])
                    bg = (mine[i][j] == -1) ? RGB(220, 60, 60) : RGB(210, 210, 210);
                else
                    bg = RGB(100, 160, 220);

                HBRUSH br = CreateSolidBrush(bg);
                FillRect(hdc, &rc, br);
                DeleteObject(br);
                FrameRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

                if (revealed[i][j])
                {
                    if (mine[i][j] == -1) {
                        SetTextColor(hdc, RGB(255, 255, 255));
                        DrawTextA(hdc, "*", -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    }
                    else if (mine[i][j] > 0) {
                        COLORREF numColor[] = {
                            RGB(0,0,0),       RGB(0,0,220),    RGB(0,128,0),
                            RGB(200,0,0),     RGB(0,0,128),    RGB(128,0,0),
                            RGB(0,128,128),   RGB(0,0,0),      RGB(100,100,100)
                        };
                        int v = mine[i][j];
                        if (v < 1 || v > 8) v = 1;
                        SetTextColor(hdc, numColor[v]);
                        char s[4];
                        sprintf(s, "%d", mine[i][j]);
                        DrawTextA(hdc, s, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                    }
                }
            }
        }

        // 顶部信息栏
        RECT top = { MARGIN, MARGIN, MARGIN + cols * CELL, MARGIN + TOPBAR - 5 };
        if (gameOver) {
            SetTextColor(hdc, win ? RGB(0, 150, 0) : RGB(200, 0, 0));
            DrawTextA(hdc, win ? "恭喜通关！按 R 重新开始" : "踩雷了！按 R 重新开始",
                -1, &top, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
        else {
            SetTextColor(hdc, RGB(0, 0, 0));
            char info[80];
            sprintf(info, "地雷 %d 个   已翻开 %d / %d   左键点格子",
                landmine, openedCount, rows * cols - landmine);
            DrawTextA(hdc, info, -1, &top, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }

        SelectObject(hdc, oldFont);
        DeleteObject(hFont);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        if (gameOver) return 0;
        int px = LOWORD(lp);
        int py = HIWORD(lp);

        int col = (px - MARGIN) / CELL;
        int row = (py - MARGIN - TOPBAR) / CELL;

        if (row < 0 || row >= rows || col < 0 || col >= cols) return 0;
        if (revealed[row][col]) return 0;

        if (mine[row][col] == -1) {
            // 踩雷：翻开所有雷，游戏结束
            gameOver = 1;
            for (int i = 0; i < rows; i++)
                for (int j = 0; j < cols; j++)
                    if (mine[i][j] == -1) revealed[i][j] = 1;
        }
        else {
            floodReveal(row, col);
            // 检查是否全部翻完
            if (openedCount == rows * cols - landmine) {
                gameOver = 1;
                win = 1;
            }
        }
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_KEYDOWN:
        if (wp == 'R' && gameOver) {
            memset(mine, 0, sizeof(mine));
            memset(revealed, 0, sizeof(revealed));
            gameOver = 0; win = 0; openedCount = 0;
            putlandmine(rows, cols, mine, landmine);
            count(rows, cols, mine);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

int main()
{
    printf("输入行数和列数（最大50）：\n");
    scanf("%d %d", &rows, &cols);
    if (rows < 1 || rows > MAXN) rows = 10;
    if (cols < 1 || cols > MAXN) cols = 10;

    printf("输入地雷个数：\n");
    scanf("%d", &landmine);
    if (landmine < 1) landmine = 1;
    if (landmine > rows * cols - 1) landmine = rows * cols - 1;

    putlandmine(rows, cols, mine, landmine);
    count(rows, cols, mine);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "MineSweeperWin";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    int winW = MARGIN * 2 + cols * CELL;
    int winH = MARGIN * 2 + TOPBAR + rows * CELL;
    RECT r = { 0, 0, winW, winH };
    AdjustWindowRect(&r, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hwnd = CreateWindowEx(
        0, "MineSweeperWin", "扫雷",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        r.right - r.left, r.bottom - r.top,
        NULL, NULL, GetModuleHandle(NULL), NULL);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}