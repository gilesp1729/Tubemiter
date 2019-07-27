#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <math.h>
#include <windows.h>

#include "resource.h"
#include "dxf.h"

/*
 * Globals. Tube 1 is the tube being mitered.
 */
HINSTANCE   hInst;		/* Instance thingy */
double	    d1 = 25.4, d2 = 25.4; /* tubing diameters */
double	    t = 1.2;		/* tube 1 wall thickness */
double	    beta = 90.0;	/* included angle between tubes */
double      offset = 0.0;       /* offset between centrelines */
BOOL	    negative = FALSE;
int	    margin = 100;	/* 10mm in LOMETRIC units */
HPEN	    gray_pen;
FILE	    *dxf;		/* DXF file to save to */
int	    units = 0;		/* 0 = mm, 1 = inch */
char	    *u[2] = {"mm", "in"}; /* units names */
double	    m[2] = {1.0, 25.4};	/* units multipliers */

/*
 * Print details.
 */
void
details(HDC hdc)
{
    char	string[128];
    HPEN	old_pen;

    TextOut
    (
	hdc, 
	margin + 10, 
	-margin - 10, 
	string,
	sprintf(string, "This tube %g %s OD (%g %s wall)", 
		d1 / m[units], u[units], t / m[units], u[units])
    );

    TextOut
    (
	hdc, 
	margin + 10, 
	-margin - 60, 
	string,
	sprintf(string, "meets %g %s OD", 
		d2 / m[units], u[units])
    );

    TextOut
    (
	hdc, 
	margin + 10, 
	-margin - 110, 
	string,
	sprintf(string, "at %g degrees, with an offset of %g %s", 
		beta, offset / m[units], u[units])
    );
    TextOut
    (
	hdc, 
	margin + 10, 
	-margin - 210, 
	string,
	sprintf(string, "Inside", beta)
    );
    TextOut
    (
	hdc, 
	margin + 10, 
	-margin - 260, 
	string,
	sprintf(string, "Outside", beta)
    );

    old_pen = SelectObject(hdc, gray_pen);
    MoveToEx(hdc, margin + 180, -margin - 280, NULL);
    LineTo(hdc, margin + 300, -margin - 280);
    SelectObject(hdc, old_pen);
    MoveToEx(hdc, margin + 180, -margin - 230, NULL);
    LineTo(hdc, margin + 300, -margin - 230);
}

/*
 * Plot the shape.
 */
void
plot(HDC hdc, double t, int root)
{
    double	d;
    double	theta = (90.0 - beta) / 57.29577;
    double	r1 = d1 / 2.0;    
    double	r2 = d2 / 2.0;    
    int		xlabel = min(2000, (int)(360/57.29577 * r1 * 10.0 * (1 + sin(theta))));
    int		ylabel = (int)(360/57.29577 * r1 * 10.0);
    int		edge = margin + (1 - root) * xlabel;
    HPEN	pen;

    SetMapMode(hdc, MM_LOMETRIC);
    if (t == 0.0)
	pen = SelectObject(hdc, gray_pen);
    else
	pen = SelectObject(hdc, GetStockObject(BLACK_PEN));
    
    MoveToEx(hdc, edge, -margin, NULL);

    for (d = 0; d <= 360; d += 5)
    {
	double  rad = d / 57.29577;
	double  rad90 = (d + 90) / 57.29577;
	double  co = cos(rad90);
	double  si = sin(rad90);
	double  xi = (r1 - t) * co - offset;
	double  yi = (r1 - t) * si;
	double  zi;

	double  ct = cos(theta);
	double  st = sin(theta);

	double  a = ct * ct;	    /* for quadratic eqn */
	double  b = 2 * yi * st * ct;
	double  c = (xi * xi) + (yi * yi * st * st) - (r2 * r2);

	double	discr = b * b - 4 * a * c;

	if (discr >= 0)
	{
	    zi = (-b + root * sqrt(discr)) / (2 * a);
	    if (!negative)
	    {
		LineTo
		(
		    hdc, 
		    margin + xlabel + (int)(r2 * 10.0) - (int)(zi * 10.0), 
		    -margin - (int)(rad * r1 * 10.0)
		);
	    }
	    else
	    {
		MoveToEx
		(
		    hdc, 
		    margin + xlabel + (int)(r2 * 10.0) - (int)(zi * 10.0), 
		    -margin - (int)(rad * r1 * 10.0),
		    NULL
		);
		negative = FALSE;
	    }
	}
	else
	{
	    negative = TRUE;
	    MoveToEx
	    (
		hdc, 
		margin + xlabel + (int)(r2 * 10.0), 
		-margin - (int)(rad * r1 * 10.0),
		NULL
	    );

	}
    }

    LineTo(hdc, edge, -margin - ylabel);
    LineTo(hdc, edge, -margin);
    SelectObject(hdc, pen);
}


/*
 * Plot the shape to a DXF file.
 */
void
plot_dxf(FILE *dxf, double t, int root)
{
    double	d;
    double	theta = (90.0 - beta) / 57.29577;
    double	r1 = d1 / 2.0;    
    double	r2 = d2 / 2.0;    
    int		xlabel = min(200, (int)(360/57.29577 * r1  * (1 + sin(theta))));
    int		ylabel = (int)(360/57.29577 * r1);
    int  	marg = margin / 10;
    int		edge = marg + (1 - root) * xlabel;
    double	x1, y1, x2, y2;

    if (t == 0.0)
	dxf_set_layer(1);
    else
	dxf_set_layer(0);
    
    x1 = edge;
    y1 = -marg;

    for (d = 0; d <= 360; d += 5)
    {
	double  rad = d / 57.29577;
	double  rad90 = (d + 90) / 57.29577;
	double  co = cos(rad90);
	double  si = sin(rad90);
	double  xi = (r1 - t) * co - offset;
	double  yi = (r1 - t) * si;
	double  zi;

	double  ct = cos(theta);
	double  st = sin(theta);

	double  a = ct * ct;	    /* for quadratic eqn */
	double  b = 2 * yi * st * ct;
	double  c = (xi * xi) + (yi * yi * st * st) - (r2 * r2);

	double	discr = b * b - 4 * a * c;

	if (discr >= 0)
	{
	    zi = (-b + root * sqrt(discr)) / (2 * a);
	    if (!negative)
	    {
		x2 = marg + xlabel + r2 - zi;
		y2 = -marg - (rad * r1);
		dxf_line(dxf, x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
	    }
	    else
	    {
		x1 = marg + xlabel + r2 - zi;
		y1 = -marg - (rad * r1);
		negative = FALSE;
	    }
	}
	else
	{
	    negative = TRUE;
	    x1 = marg + xlabel + r2;
	    y1 = -marg - (rad * r1);
	}
    }
    dxf_line(dxf, x1, y1, edge, -marg - ylabel);
    dxf_line(dxf, edge, -marg - ylabel, edge, -marg);
}


/*
 * "About" dialog procedure.
 */
int WINAPI
AboutDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDOK:
	    EndDialog(hWnd, TRUE);
	    break;

        case IDCANCEL:
	    EndDialog(hWnd, FALSE);
	    break;

	default:
	    return 0;
	}
	
    default:
	return 0;
    }

    return 1;
}


/*
 * Settings dialog procedure.
 */
int WINAPI
SettingsDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    char    string[40];
    char    *end;
    double  temp;
    int	    ctrl;

    switch (msg)
    {
    case WM_INITDIALOG:
	SetWindowText(hWnd, "Tube miter settings");

	sprintf(string, "%g", d1 / m[units]);
	SetDlgItemText(hWnd, IDC_D1, string);
	sprintf(string, "%g", t / m[units]);
	SetDlgItemText(hWnd, IDC_THICK, string);
	sprintf(string, "%g", d2 / m[units]);
	SetDlgItemText(hWnd, IDC_D2, string);
	sprintf(string, "%g", beta);
	SetDlgItemText(hWnd, IDC_BETA, string);
	sprintf(string, "%g", offset / m[units]);
	SetDlgItemText(hWnd, IDC_OFFSET, string);
        SendDlgItemMessage(hWnd, IDC_INCHES, BM_SETCHECK, units ? BST_CHECKED : BST_UNCHECKED, 0);

	break;

    case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDOK:
	    ctrl = IDC_D1;
	    GetDlgItemText(hWnd, ctrl, string, sizeof(string));
	    if ((temp = strtod(string, &end)) <= 0.0 || *end != '\0')
		goto beep;
	    d1 = temp * m[units];

	    ctrl = IDC_THICK;
	    GetDlgItemText(hWnd, ctrl, string, sizeof(string));
	    if ((temp = strtod(string, &end)) < 0.0 || *end != '\0')
		goto beep;
	    t = temp * m[units];

	    ctrl = IDC_D2;
	    GetDlgItemText(hWnd, ctrl, string, sizeof(string));
	    if ((temp = strtod(string, &end)) <= 0.0 || *end != '\0')
		goto beep;
	    d2 = temp * m[units];

	    ctrl = IDC_BETA;
	    GetDlgItemText(hWnd, ctrl, string, sizeof(string));
	    if ((temp = strtod(string, &end)) <= 0.0 || *end != '\0')
		goto beep;
	    beta = temp;

	    ctrl = IDC_OFFSET;
	    GetDlgItemText(hWnd, ctrl, string, sizeof(string));
            temp = strtod(string, &end);
	    if (*end != '\0')
		goto beep;
	    offset = temp * m[units];

	    EndDialog(hWnd, TRUE);
	    break;

	beep:
	    SendDlgItemMessage(hWnd, ctrl, EM_SETSEL, 0, -1);
	    MessageBeep(0);
	    break;

	case IDCANCEL:
	    EndDialog(hWnd, FALSE);
	    break;

	case IDC_INCHES:
	    units = 
		(IsDlgButtonChecked(hWnd, IDC_INCHES) == BST_CHECKED)
		?
		1 : 0;
	    sprintf(string, "%g", d1 / m[units]);
	    SetDlgItemText(hWnd, IDC_D1, string);
	    sprintf(string, "%g", t / m[units]);
	    SetDlgItemText(hWnd, IDC_THICK, string);
	    sprintf(string, "%g", d2 / m[units]);
	    SetDlgItemText(hWnd, IDC_D2, string);
	    sprintf(string, "%g", offset / m[units]);
	    SetDlgItemText(hWnd, IDC_OFFSET, string);
	    break;

	default:
	    return 0;
	}
	
    default:
	return 0;
    }
    
    return 1;
}


/*
 * Main window procedure.
 */
int WINAPI
MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT	ps;
    HDC		hdc;
    PRINTDLG	pd;
    DOCINFO	di;
    OPENFILENAME    ofn;
    char            dxf_file[128];
    char            file_title[64];

    switch (msg)
    {
    case WM_CREATE:
	gray_pen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));

	break;

    case WM_PAINT:
	hdc = BeginPaint(hWnd, &ps);
	plot(hdc, 0.0, 1);
	plot(hdc, t, 1);
	if (d1 > d2)
	{
	    plot(hdc, 0.0, -1);
	    plot(hdc, t, -1);
	}
	details(hdc);
	EndPaint(hWnd, &ps);

	break;

    case WM_COMMAND:
	switch (LOWORD(wParam))
	{
	case IDC_SETTINGS:
	    DialogBox
	    (
		hInst, 
		MAKEINTRESOURCE(IDD_SETTINGS), 
		hWnd,
		SettingsDlgProc
	    );

	    InvalidateRect(hWnd, NULL, TRUE);
	    break;

	case IDC_ABOUT:
	    DialogBox
	    (
		hInst, 
		MAKEINTRESOURCE(IDD_ABOUT), 
		hWnd,
		AboutDlgProc
	    );
            break;

	case IDC_SAVE_DXF:
            dxf_file[0] = '\0';
            memset(&ofn, 0, sizeof(OPENFILENAME));
	    ofn.lStructSize = sizeof(OPENFILENAME);
	    ofn.hwndOwner = hWnd;
	    ofn.lpstrFilter = "DXF files (*.dxf)\0*.dxf\0All Files (*.*)\0*.*\0";
	    ofn.lpstrTitle = "Save to DXF File";
	    ofn.lpstrFile = dxf_file;
	    ofn.nMaxFile = sizeof(dxf_file);
	    ofn.lpstrFileTitle = file_title;
	    ofn.nMaxFileTitle = sizeof(file_title);
	    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	    ofn.lpstrDefExt = "dxf";

	    if (!GetSaveFileName(&ofn))
		break;

            dxf = dxf_open(dxf_file);
	    plot_dxf(dxf, 0.0, 1);
	    plot_dxf(dxf, t, 1);
	    if (d1 > d2)
	    {
		plot_dxf(dxf, 0.0, -1);
		plot_dxf(dxf, t, -1);
	    }
	    dxf_close(dxf);
	    break;

	case IDC_PRINT:
	    memset(&pd, 0, sizeof(pd));
	    pd.lStructSize = sizeof(pd);
	    pd.hwndOwner = hWnd;
	    pd.Flags = PD_RETURNDC;

	    PrintDlg(&pd);

	    memset(&di, 0, sizeof(di));
	    di.cbSize = sizeof(di);
	    di.lpszDocName = "Tubemiter";

	    StartDoc(pd.hDC, &di);
	    StartPage(pd.hDC);
   	    
   	    plot(pd.hDC, 0.0, 1);
	    plot(pd.hDC, t, 1);
    	    if (d1 > d2)
	    {
		plot(pd.hDC, 0.0, -1);
		plot(pd.hDC, t, -1);
	    }
	    details(pd.hDC);

	    EndPage(pd.hDC);
	    EndDoc(pd.hDC);

	    break;

	case IDC_EXIT:
	    /*
	     * Exiting.
	     */
	    goto get_out;
	}
	break;

    case WM_DESTROY:
get_out:
	PostQuitMessage(0);
	break;

    default:
	return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 1;
}


int WINAPI 
WinMain
(
    HINSTANCE hInstance,	// handle to current instance
    HINSTANCE hPrevInstance,	// handle to previous instance
    LPSTR lpCmdLine,	// pointer to command line
    int nCmdShow 	// show state of window
)
{
    MSG		msg; 
    WNDCLASS	wc; 
    HWND	hwndMain;
    
    /* Register the window class for the main window. */ 
 
    if (!hPrevInstance) 
    { 
        wc.style = 0; 
        wc.lpfnWndProc = (WNDPROC)MainWndProc; 
        wc.cbClsExtra = 0; 
        wc.cbWndExtra = 0; 
        wc.hInstance = hInstance; 
        wc.hIcon = LoadIcon((HINSTANCE) NULL, 
            IDI_APPLICATION); 
        wc.hCursor = LoadCursor((HINSTANCE) NULL, 
            IDC_ARROW); 
        wc.hbrBackground = GetStockObject(WHITE_BRUSH); 
        wc.lpszMenuName =  MAKEINTRESOURCE(IDR_MENU1); 
        wc.lpszClassName = "TubeWndClass"; 
 
        if (!RegisterClass(&wc)) 
            return FALSE; 
    } 
 
    hInst = hInstance;  /* save instance handle */ 
 
    /* Create the main window. */ 
 
    hwndMain = CreateWindow("TubeWndClass", "Tubemiter", 
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
        CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL, 
        (HMENU) NULL, hInst, (LPVOID) NULL); 
 
    /* 
     * If the main window cannot be created, terminate 
     * the application. 
     */ 
 
    if (!hwndMain) 
        return FALSE; 
 
    /* Show the window and paint its contents. */ 
 
    ShowWindow(hwndMain, nCmdShow); 
    UpdateWindow(hwndMain); 

    /* Start the message loop. */ 
 
    while (GetMessage(&msg, (HWND) NULL, 0, 0)) 
    { 
        TranslateMessage(&msg); 
        DispatchMessage(&msg); 
    } 
 
    /* Return the exit code to Windows. */ 
 
    return msg.wParam; 
}
