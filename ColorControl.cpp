//////////////////////////////////////////////////////////////////////////////
//
// File name : ColorControl.cpp
// Date		 : May. 13. 2008.
// Author    : Sang Tae, Lee
// Purpose	 :
//			   Static Control���� Color�� ����ϱ� ���� Subclassing.
//
//////////////////////////////////////////////////////////////////////////////
// ColorControl.cpp : implementation file
//

#include "stdafx.h"
#include "ColorControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////
// CColorControl

CColorControl::CColorControl()
{
	// Set default parent window and notification message
	m_pParent = NULL;
	m_nMsg = WM_USER;

	// By default the control is not blinking
	m_bTextBlink = FALSE;
	m_nTextBlinkStep = 0;
	m_bBkBlink = FALSE;
	m_nBkBlinkStep = 0;
	m_nTimerId = 0;

	// Set default foreground text
	m_crTextColor = ::GetSysColor(COLOR_BTNTEXT);

	// Set default foreground text (when blinking)
	m_crBlinkTextColors[0] = m_crTextColor;
	m_crBlinkTextColors[1] = m_crTextColor;

	// Set default background text
	m_crBkColor = ::GetSysColor(COLOR_BTNFACE);

	// Set default background text (when blinking)
	m_crBlinkBkColors[0] = m_crBkColor;
	m_crBlinkBkColors[1] = m_crBkColor;

	// Set default background brush
	m_brBkgnd.CreateSolidBrush(m_crBkColor);

	// Set default background brush (when blinking)
	m_brBlinkBkgnd[0].CreateSolidBrush(m_crBkColor);
	m_brBlinkBkgnd[1].CreateSolidBrush(m_crBkColor);
}

CColorControl::~CColorControl()
{
}


BEGIN_MESSAGE_MAP(CColorControl, CStatic)
	//{{AFX_MSG_MAP(CColorControl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorControl message handlers

HBRUSH CColorControl::CtlColor(CDC *pDC, UINT nCtlColor)
{
	// Set foreground color
	// If control is blinking (text)
	if (m_bTextBlink == TRUE)
	{
		pDC->SetTextColor(m_crBlinkTextColors[m_nTextBlinkStep]);
	}
	else
	{
		pDC->SetTextColor(m_crTextColor);
	}

	// Set background color & brush
	// If control is blinking (background)
	if (m_bBkBlink == TRUE)
	{
		pDC->SetBkColor(m_crBlinkBkColors[m_nBkBlinkStep]);
		return (HBRUSH)m_brBlinkBkgnd[m_nBkBlinkStep];
	}
	// If control is not blinking (background)
	pDC->SetBkColor(m_crBkColor);

	// Return a non-NULL brush if the parent's handler should not be called
    return (HBRUSH)m_brBkgnd;
}

void CColorControl::OnDestroy() 
{
	CStatic::OnDestroy();
	
	// Destroy timer (if any)
	if (m_nTimerId > 0) KillTimer(m_nTimerId);

	// Destroy resources
    m_brBkgnd.DeleteObject();
    m_brBlinkBkgnd[0].DeleteObject();
    m_brBlinkBkgnd[1].DeleteObject();
}

void CColorControl::SetTextColor(COLORREF crTextColor)
{
	// Set new foreground color
	if (crTextColor != 0xffffffff)
	{
		m_crTextColor = crTextColor;
	}
	else // Set default foreground color
	{
		m_crTextColor = ::GetSysColor(COLOR_BTNTEXT);
	}

	// Repaint control
	Invalidate();
}

void CColorControl::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimerId)	
	{
		// If control is blinking (text) switch its color
		if (m_bTextBlink == TRUE) m_nTextBlinkStep = !m_nTextBlinkStep;

		// If control is blinking (background) switch its color
		if (m_bBkBlink == TRUE) m_nBkBlinkStep = !m_nBkBlinkStep;

		// If there is any blinking in action then repaint the control
		// and send the notification message (if any)
		if (m_bBkBlink == TRUE || m_bTextBlink == TRUE) 
		{
			Invalidate();
			// Send notification message only on rising blink
			if (m_pParent != NULL && (m_nBkBlinkStep == 1 || m_nTextBlinkStep == 1)) 
				m_pParent->PostMessage(m_nMsg, GetDlgCtrlID(), 0);
		}
	}
	else
	CStatic::OnTimer(nIDEvent);
}

void CColorControl::SetBkColor(COLORREF crBkColor)
{
	// Set new background color
	if (crBkColor != 0xffffffff)
	{
		m_crBkColor = crBkColor;
	}
	else // Set default background color
	{
		m_crBkColor = ::GetSysColor(COLOR_BTNFACE);
	}

    m_brBkgnd.DeleteObject();
    m_brBkgnd.CreateSolidBrush(m_crBkColor);

	// Repaint control
	Invalidate();
}

COLORREF CColorControl::GetTextColor()
{
	return m_crTextColor;
}

void CColorControl::SetBlinkBkColors(COLORREF crBlinkBkColor1, COLORREF crBlinkBkColor2)
{
	// Set new blink background colors
	m_crBlinkBkColors[0] = crBlinkBkColor1;
	m_crBlinkBkColors[1] = crBlinkBkColor2;

	m_brBlinkBkgnd[0].DeleteObject();
  m_brBlinkBkgnd[0].CreateSolidBrush(m_crBlinkBkColors[0]);
  m_brBlinkBkgnd[1].DeleteObject();
  m_brBlinkBkgnd[1].CreateSolidBrush(m_crBlinkBkColors[1]);

	// Repaint control
	Invalidate();
}

void CColorControl::SetBlinkTextColors(COLORREF crBlinkTextColor1, COLORREF crBlinkTextColor2)
{
	// Set new blink text colors
	m_crBlinkTextColors[0] = crBlinkTextColor1;
	m_crBlinkTextColors[1] = crBlinkTextColor2;
}

void CColorControl::StartTextBlink(BOOL bStart, UINT nElapse)
{
	UINT nCount;

	// Destroy any previous timer
	if (m_nTimerId > 0)	
	{
		KillTimer(m_nTimerId);
		m_nTimerId = 0;
	}

	m_bTextBlink = bStart;
	m_nTextBlinkStep = 0;

	// Start timer
	if (m_bTextBlink == TRUE) 
	{
		switch (nElapse)
		{
			case ST_FLS_SLOW:
				nCount = 1500;
				break;
			case ST_FLS_NORMAL:
				nCount = 1000;
				break;
			case ST_FLS_FAST:
				nCount = 500;
				break;
			default:
				nCount = nElapse;
				break;
		}
		m_nTimerId = (UINT)SetTimer(1, nCount, NULL); 
	}
}

void CColorControl::StartBkBlink(BOOL bStart, UINT nElapse)
{
	UINT nCount;

	// Destroy any previous timer
	if (m_nTimerId > 0)	
	{
		KillTimer(m_nTimerId);
		m_nTimerId = 0;
	}

	m_bBkBlink = bStart;
	m_nBkBlinkStep = 0;

	// Start timer
	if (m_bBkBlink == TRUE) 
	{
		switch (nElapse)
		{
			case ST_FLS_SLOW:
				nCount = 1500;
				break;
			case ST_FLS_NORMAL:
				nCount = 1000;
				break;
			case ST_FLS_FAST:
				nCount = 500;
				break;
			default:
				nCount = nElapse;
				break;
		}
		m_nTimerId = (UINT)SetTimer(1, nCount, NULL);
	}
}

COLORREF CColorControl::GetBkColor()
{
	return m_crBkColor;
}

void CColorControl::EnableNotify(CWnd *pParent, UINT nMsg)
{
	m_pParent = pParent;
	m_nMsg = nMsg;
}