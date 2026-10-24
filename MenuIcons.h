#pragma once

#include <afxwin.h>

class CMenuIconManager
{
public:
	CMenuIconManager();
	~CMenuIconManager();

	BOOL ApplyToPopup(HMENU hRootMenu, HMENU hPopupMenu, UINT dpi);
	void Clear(HMENU hRootMenu);

private:
	struct MenuCommandIcon
	{
		UINT commandId;
		WCHAR glyph;
		WCHAR overlayGlyph;
		HBITMAP enabledBitmap;
		HBITMAP disabledBitmap;
	};

	static const size_t CommandCount = 14;

	BOOL EnsureBitmaps(HMENU hRootMenu, UINT dpi, COLORREF textColor, COLORREF disabledColor);
	BOOL CreateIconBitmaps(MenuCommandIcon& icon, UINT dpi, COLORREF textColor, COLORREF disabledColor);
	void ClearMenuBitmaps(HMENU hMenu);
	void DeleteBitmaps();

	MenuCommandIcon m_icons[CommandCount];
	UINT m_dpi;
	COLORREF m_textColor;
	COLORREF m_disabledColor;
};
