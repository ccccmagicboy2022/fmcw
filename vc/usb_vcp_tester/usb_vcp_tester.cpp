#include "StdAfx.h"

bool Close_Exit = false;

bool Ctrlhandler( DWORD fdwctrltype )
{
    switch( fdwctrltype )
    {
    // handle the ctrl-c signal.
    case CTRL_C_EVENT:
        printf( "ctrl-c event\n\n" );
        return( true );
    // ctrl-close: confirm that the user wants to exit.
    case CTRL_CLOSE_EVENT:
        //控制台结束时 要做的事情
        Close_Exit = true ;
        Sleep(200);
        printf( "ctrl-close event\n\n" );
        TRACE("ctrl-close event\n\n");
        return( true );
    // pass other signals to the next handler.
    case CTRL_BREAK_EVENT:
        Close_Exit = true ;
        Sleep(200);
        printf( "ctrl-break event\n\n" );
        return false;
    case CTRL_LOGOFF_EVENT:
        Close_Exit = true ;
        Sleep(200);
        printf( "ctrl-logoff event\n\n" );
        return false;
    case CTRL_SHUTDOWN_EVENT:
        Close_Exit = true ;
        Sleep(200);
        printf( "ctrl-shutdown event\n\n" );
        return false;
    default:
        return false;
    }
}

int main()
{
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)Ctrlhandler, true);

    HANDLE hComm;
	CString	port_alis;
	CString	error_message;
	DWORD	dwError;
	unsigned char	szBuffer_r[4096];
	BOOL	fStat;
    unsigned	long	rev_num;
    CString	temp_str;

    unsigned long passed_num = 0;

	port_alis.Format("\\\\.\\%s", "COM11");
	TRACE(port_alis);

    hComm = CreateFileA((LPSTR)(LPCTSTR)(port_alis),GENERIC_READ | GENERIC_WRITE, 
		0, 
		0, 
		OPEN_EXISTING,
		NULL,
        0);

    if (hComm == INVALID_HANDLE_VALUE)
    {
		dwError = GetLastError();
		
		error_message.Format("com port open error! - %d", dwError);
        ::MessageBox(::GetFocus(), (LPSTR)(LPCTSTR)error_message, NULL, MB_OK|MB_ICONERROR|MB_SYSTEMMODAL);
        return -1;
    }

	//block rev
	COMMTIMEOUTS cto;
	GetCommTimeouts(hComm,&cto);
	// Set the new timeouts
	cto.ReadIntervalTimeout = 0;
	cto.ReadTotalTimeoutConstant = 0;
	cto.ReadTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hComm,&cto);

    memset(szBuffer_r, 0, sizeof(szBuffer_r));
	while(1)
	{
        if (Close_Exit)
        {
            break;
        }

	    fStat	=	ReadFile(hComm, szBuffer_r, 0x1, &rev_num, NULL);

        if (fStat == 0x01)
        {
	        if (0x1	==	rev_num)
	        {
                if (szBuffer_r[0] == 0xAB)
                {
		            temp_str.Format("0x%02X", szBuffer_r[0]);
		            //TRACE(temp_str);
                    fStat	=	ReadFile(hComm, szBuffer_r, 0x1, &rev_num, NULL);
                    if (fStat == 0x01)
                    {
                        if (0x1	==	rev_num)
                        {
                            if (szBuffer_r[0] == 0xCD)
                            {
		                        temp_str.Format("0x%02X", szBuffer_r[0]);
		                        //TRACE(temp_str);
                                fStat	=	ReadFile(hComm, szBuffer_r, 4096, &rev_num, NULL);
                                if (4096	==	rev_num)
                                {
                                    //TRACE("PACKET");
                                    //printf("PACKET\r\n");
                                    for (int i=0;i<4096;i++)
                                    {
                                        if ( szBuffer_r[i] == i % 0x100)
                                        {
                                            //pass
                                        }
                                        else
                                        {
                                            TRACE("ERROR!");
                                            Close_Exit = true;
                                            break;
                                        }
                                    }
                                    
                                    temp_str.Format("pass: %d\r\n", passed_num++);
                                    printf(temp_str);
                                    TRACE(temp_str);
                                }
                            }
                        }
                    }
                }
	        }        
        }
	}

    CloseHandle(hComm);
	return 0;
}








