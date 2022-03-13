#include"control.hpp"

HANDLE hIn=GetStdHandle(STD_INPUT_HANDLE);
HANDLE hOut=GetStdHandle(STD_OUTPUT_HANDLE);
void gotoxy(SHORT x, SHORT y)
{
    HANDLE hout=GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord={y, x};
    SetConsoleCursorPosition(hout, coord);
}

void sred(){SetConsoleTextAttribute(hOut,Red|(White<<4));}
void sblack(){SetConsoleTextAttribute(hOut,White<<4);}
void cls(){
    sblack();
    gotoxy(0,0);
    std::string blank="",blank0="                                                                                                    ";
    for (int i=1; i<=25; i++) blank+=blank0;// 100 spaces
    std::cout<<blank;
    gotoxy(0,0);
    
}
void cp(){
    SetConsoleTitle (L"斗主地");
    SetConsoleCP(65001);
    sblack();
    system("mode con lines=25 cols=100");
    cls();
    DWORD consoleMode;
	GetConsoleMode(hIn,&consoleMode);
	SetConsoleMode(hIn,consoleMode|ENABLE_MOUSE_INPUT);
    CONSOLE_CURSOR_INFO cinfo={1,0};
    SetConsoleCursorInfo(hOut,&cinfo);
	SetConsoleOutputCP(65001);
}