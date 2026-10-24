#include "stdafx.h"
#include "MenuIcons.h"
#include "resource.h"

#include <algorithm>
#include <cstring>

namespace
{
	// Use the built-in Windows symbol font to avoid external icon dependencies.
	constexpr WCHAR SegoeMdl2Assets[] = L"Segoe MDL2 Assets";
	constexpr WCHAR GlyphAdd = L'\xE710';
	constexpr WCHAR GlyphSettings = L'\xE713';
	constexpr WCHAR GlyphSearch = L'\xE721';
	constexpr WCHAR GlyphSave = L'\xE74E';
	constexpr WCHAR GlyphPaste = L'\xE77F';
	constexpr WCHAR GlyphSaveAs = L'\xE792';
	constexpr WCHAR GlyphUndo = L'\xE7A7';
	constexpr WCHAR GlyphDocument = L'\xE8A5';
	constexpr WCHAR GlyphClose = L'\xE8BB';
	constexpr WCHAR GlyphCut = L'\xE8C6';
	constexpr WCHAR GlyphCopy = L'\xE8C8';
	constexpr WCHAR GlyphOpenFile = L'\xE8E5';
	constexpr WCHAR GlyphHelp = L'\xE897';
	constexpr WCHAR GlyphInfo = L'\xE946';
	constexpr WCHAR GlyphPrint = L'\xE749';

	HBITMAP CreateGlyphBitmap(WCHAR glyph, WCHAR overlayGlyph, UINT dpi, COLORREF color)
	{
		const int size = MulDiv(16, static_cast<int>(dpi), 96);
		if (size <= 0)
			return nullptr;

		BITMAPINFO bitmapInfo = {};
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biWidth = size;
		bitmapInfo.bmiHeader.biHeight = -size;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;

		void* maskPixels = nullptr;
		void* iconPixels = nullptr;
		HBITMAP maskBitmap = CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS, &maskPixels, nullptr, 0);
		HBITMAP iconBitmap = CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS, &iconPixels, nullptr, 0);
		if (!maskBitmap || !iconBitmap || !maskPixels || !iconPixels)
		{
			if (maskBitmap)
				DeleteObject(maskBitmap);
			if (iconBitmap)
				DeleteObject(iconBitmap);
			return nullptr;
		}

		const size_t pixelBytes = static_cast<size_t>(size) * static_cast<size_t>(size) * 4;
		std::memset(maskPixels, 0, pixelBytes);
		std::memset(iconPixels, 0, pixelBytes);

		HDC maskDC = CreateCompatibleDC(nullptr);
		if (!maskDC)
		{
			DeleteObject(maskBitmap);
			DeleteObject(iconBitmap);
			return nullptr;
		}

		HGDIOBJ oldBitmap = SelectObject(maskDC, maskBitmap);
		HFONT font = CreateFontW(
			-size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, SegoeMdl2Assets);
		if (!font)
		{
			SelectObject(maskDC, oldBitmap);
			DeleteDC(maskDC);
			DeleteObject(maskBitmap);
			DeleteObject(iconBitmap);
			return nullptr;
		}

		HGDIOBJ oldFont = SelectObject(maskDC, font);
		WORD glyphIndex = 0;
		if (GetGlyphIndicesW(maskDC, &glyph, 1, &glyphIndex, GGI_MARK_NONEXISTING_GLYPHS) == GDI_ERROR ||
			glyphIndex == 0xFFFF)
		{
			SelectObject(maskDC, oldFont);
			SelectObject(maskDC, oldBitmap);
			DeleteObject(font);
			DeleteDC(maskDC);
			DeleteObject(maskBitmap);
			DeleteObject(iconBitmap);
			return nullptr;
		}

		SetBkMode(maskDC, TRANSPARENT);
		SetTextColor(maskDC, RGB(255, 255, 255));

		RECT glyphRect = { 0, 0, size, size };
		DrawTextW(maskDC, &glyph, 1, &glyphRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

		BOOL overlayRendered = TRUE;
		if (overlayGlyph != 0)
		{
			const int overlaySize = MulDiv(9, static_cast<int>(dpi), 96);
			HFONT overlayFont = CreateFontW(
				-overlaySize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
				DEFAULT_PITCH | FF_DONTCARE, SegoeMdl2Assets);
			if (overlayFont)
			{
				SelectObject(maskDC, overlayFont);
				WORD overlayGlyphIndex = 0;
				if (GetGlyphIndicesW(maskDC, &overlayGlyph, 1, &overlayGlyphIndex, GGI_MARK_NONEXISTING_GLYPHS) != GDI_ERROR &&
					overlayGlyphIndex != 0xFFFF)
				{
					RECT overlayRect = { size / 2, size / 2, size, size };
					DrawTextW(maskDC, &overlayGlyph, 1, &overlayRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
				}
				else
				{
					overlayRendered = FALSE;
				}
				SelectObject(maskDC, font);
				DeleteObject(overlayFont);
			}
			else
			{
				overlayRendered = FALSE;
			}
		}

		SelectObject(maskDC, oldFont);
		SelectObject(maskDC, oldBitmap);
		DeleteObject(font);
		DeleteDC(maskDC);

		const BYTE red = GetRValue(color);
		const BYTE green = GetGValue(color);
		const BYTE blue = GetBValue(color);
		const BYTE* source = static_cast<const BYTE*>(maskPixels);
		BYTE* destination = static_cast<BYTE*>(iconPixels);
		BOOL hasCoverage = FALSE;

		for (size_t offset = 0; offset < pixelBytes; offset += 4)
		{
			const BYTE coverage = (std::max)(source[offset], (std::max)(source[offset + 1], source[offset + 2]));
			destination[offset] = static_cast<BYTE>((static_cast<unsigned int>(blue) * coverage) / 255);
			destination[offset + 1] = static_cast<BYTE>((static_cast<unsigned int>(green) * coverage) / 255);
			destination[offset + 2] = static_cast<BYTE>((static_cast<unsigned int>(red) * coverage) / 255);
			destination[offset + 3] = coverage;
			hasCoverage = hasCoverage || coverage != 0;
		}

		DeleteObject(maskBitmap);
		if (!hasCoverage || !overlayRendered)
		{
			DeleteObject(iconBitmap);
			return nullptr;
		}

		return iconBitmap;
	}
}

CMenuIconManager::CMenuIconManager()
	: m_dpi(0),
	  m_textColor(CLR_INVALID),
	  m_disabledColor(CLR_INVALID)
{
	const MenuCommandIcon icons[CommandCount] =
	{
		{ ID_FILE_NEW, GlyphDocument, GlyphAdd, nullptr, nullptr },
		{ ID_FILE_OPEN, GlyphOpenFile, 0, nullptr, nullptr },
		{ ID_FILE_SAVE, GlyphSave, 0, nullptr, nullptr },
		{ ID_FILE_SAVE_AS, GlyphSaveAs, 0, nullptr, nullptr },
		{ ID_FILE_PRINT, GlyphPrint, 0, nullptr, nullptr },
		{ ID_FILE_PRINT_PREVIEW, GlyphPrint, GlyphSearch, nullptr, nullptr },
		{ ID_FILE_PRINT_SETUP, GlyphPrint, GlyphSettings, nullptr, nullptr },
		{ ID_APP_EXIT, GlyphClose, 0, nullptr, nullptr },
		{ ID_EDIT_UNDO, GlyphUndo, 0, nullptr, nullptr },
		{ ID_EDIT_CUT, GlyphCut, 0, nullptr, nullptr },
		{ ID_EDIT_COPY, GlyphCopy, 0, nullptr, nullptr },
		{ ID_EDIT_PASTE, GlyphPaste, 0, nullptr, nullptr },
		{ ID_HELP_CONTENTS, GlyphHelp, 0, nullptr, nullptr },
		{ ID_APP_ABOUT, GlyphInfo, 0, nullptr, nullptr }
	};
	std::memcpy(m_icons, icons, sizeof(m_icons));
}

CMenuIconManager::~CMenuIconManager()
{
	DeleteBitmaps();
}

BOOL CMenuIconManager::ApplyToPopup(HMENU hRootMenu, HMENU hPopupMenu, UINT dpi)
{
	if (!hRootMenu || !hPopupMenu)
		return FALSE;

	if (dpi == 0)
		dpi = 96;

	const COLORREF textColor = GetSysColor(COLOR_MENUTEXT);
	const COLORREF disabledColor = GetSysColor(COLOR_GRAYTEXT);
	if (!EnsureBitmaps(hRootMenu, dpi, textColor, disabledColor))
		return FALSE;

	const int itemCount = GetMenuItemCount(hPopupMenu);
	if (itemCount < 0)
		return FALSE;

	for (int position = 0; position < itemCount; ++position)
	{
		const UINT commandId = GetMenuItemID(hPopupMenu, position);
		if (commandId == static_cast<UINT>(-1))
			continue;

		MenuCommandIcon* icon = nullptr;
		for (size_t index = 0; index < CommandCount; ++index)
		{
			if (m_icons[index].commandId == commandId)
			{
				icon = &m_icons[index];
				break;
			}
		}
		if (!icon)
			continue;

		const UINT state = GetMenuState(hPopupMenu, position, MF_BYPOSITION);
		MENUITEMINFOW menuItem = {};
		menuItem.cbSize = sizeof(menuItem);
		menuItem.fMask = MIIM_BITMAP;
		menuItem.hbmpItem = (state & (MF_DISABLED | MF_GRAYED)) ? icon->disabledBitmap : icon->enabledBitmap;
		if (!SetMenuItemInfoW(hPopupMenu, position, TRUE, &menuItem))
			return FALSE;
	}

	return TRUE;
}

void CMenuIconManager::Clear(HMENU hRootMenu)
{
	if (hRootMenu)
		ClearMenuBitmaps(hRootMenu);
	DeleteBitmaps();
	m_dpi = 0;
	m_textColor = CLR_INVALID;
	m_disabledColor = CLR_INVALID;
}

BOOL CMenuIconManager::EnsureBitmaps(
	HMENU hRootMenu, UINT dpi, COLORREF textColor, COLORREF disabledColor)
{
	if (m_dpi == dpi && m_textColor == textColor && m_disabledColor == disabledColor)
		return TRUE;

	ClearMenuBitmaps(hRootMenu);
	DeleteBitmaps();
	m_dpi = 0;

	for (size_t index = 0; index < CommandCount; ++index)
	{
		if (!CreateIconBitmaps(m_icons[index], dpi, textColor, disabledColor))
		{
			DeleteBitmaps();
			return FALSE;
		}
	}

	m_dpi = dpi;
	m_textColor = textColor;
	m_disabledColor = disabledColor;
	return TRUE;
}

BOOL CMenuIconManager::CreateIconBitmaps(
	MenuCommandIcon& icon, UINT dpi, COLORREF textColor, COLORREF disabledColor)
{
	icon.enabledBitmap = CreateGlyphBitmap(icon.glyph, icon.overlayGlyph, dpi, textColor);
	icon.disabledBitmap = CreateGlyphBitmap(icon.glyph, icon.overlayGlyph, dpi, disabledColor);
	if (icon.enabledBitmap && icon.disabledBitmap)
		return TRUE;

	if (icon.enabledBitmap)
	{
		DeleteObject(icon.enabledBitmap);
		icon.enabledBitmap = nullptr;
	}
	if (icon.disabledBitmap)
	{
		DeleteObject(icon.disabledBitmap);
		icon.disabledBitmap = nullptr;
	}
	return FALSE;
}

void CMenuIconManager::ClearMenuBitmaps(HMENU hMenu)
{
	const int itemCount = GetMenuItemCount(hMenu);
	if (itemCount < 0)
		return;

	for (int position = 0; position < itemCount; ++position)
	{
		const UINT commandId = GetMenuItemID(hMenu, position);
		if (commandId == static_cast<UINT>(-1))
		{
			HMENU hSubMenu = GetSubMenu(hMenu, position);
			if (hSubMenu)
				ClearMenuBitmaps(hSubMenu);
			continue;
		}

		for (size_t index = 0; index < CommandCount; ++index)
		{
			if (m_icons[index].commandId == commandId)
			{
				MENUITEMINFOW menuItem = {};
				menuItem.cbSize = sizeof(menuItem);
				menuItem.fMask = MIIM_BITMAP;
				menuItem.hbmpItem = nullptr;
				SetMenuItemInfoW(hMenu, position, TRUE, &menuItem);
				break;
			}
		}
	}
}

void CMenuIconManager::DeleteBitmaps()
{
	for (size_t index = 0; index < CommandCount; ++index)
	{
		if (m_icons[index].enabledBitmap)
		{
			DeleteObject(m_icons[index].enabledBitmap);
			m_icons[index].enabledBitmap = nullptr;
		}
		if (m_icons[index].disabledBitmap)
		{
			DeleteObject(m_icons[index].disabledBitmap);
			m_icons[index].disabledBitmap = nullptr;
		}
	}
}
