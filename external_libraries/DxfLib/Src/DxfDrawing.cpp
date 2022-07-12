#include "DxfDrawing.hpp"

#include <cmath>


/*
// NOTE on colors in a DXF file:
if a color is not given (stored as -1 in Dxf::Primitive::mColor), it means ByLayer
if color number == 0, it means ByBlock
valid color indices are in the range 1-254
A DXF color palette is provided as a global const
*/





namespace Dxf
{




const uint32_t gColors[] =
{
	// basic colors:
	/*   0 */ 0x000000,  // ByBlock - use black for display
	/*   1 */ 0x0000ff,  // red
	/*   2 */ 0x00ffff,  // yellow
	/*   3 */ 0x00ff00,  // green
	/*   4 */ 0xffff00,  // cyan
	/*   5 */ 0xff0000,  // blue
	/*   6 */ 0xff00ff,  // magenta
	/*   7 */ 0xffffff,  // white (black on white?)
	/*   8 */ 0x7f7f7f,  // dk gray
	/*   9 */ 0xcfcfcf,  // lt gray

	// colorband:
	/*  10 */ 0x0000FF,
	/*  11 */ 0x7F7FFF,
	/*  12 */ 0x0000CC,
	/*  13 */ 0x6666CC,
	/*  14 */ 0x000099,
	/*  15 */ 0x4C4C99,
	/*  16 */ 0x00007F,
	/*  17 */ 0x3F3F7F,
	/*  18 */ 0x00004C,
	/*  19 */ 0x26264C,
	/*  20 */ 0x003FFF,
	/*  21 */ 0x7F9FFF,
	/*  22 */ 0x0033CC,
	/*  23 */ 0x667FCC,
	/*  24 */ 0x002699,
	/*  25 */ 0x4C5F99,
	/*  26 */ 0x001F7F,
	/*  27 */ 0x3F4F7F,
	/*  28 */ 0x00134C,
	/*  29 */ 0x262F4C,
	/*  30 */ 0x007FFF,
	/*  31 */ 0x7FBFFF,
	/*  32 */ 0x0066CC,
	/*  33 */ 0x6699CC,
	/*  34 */ 0x004C99,
	/*  35 */ 0x4C7299,
	/*  36 */ 0x003F7F,
	/*  37 */ 0x3F5F7F,
	/*  38 */ 0x00264C,
	/*  39 */ 0x26394C,
	/*  40 */ 0x00BFFF,
	/*  41 */ 0x7FDFFF,
	/*  42 */ 0x0099CC,
	/*  43 */ 0x66B2CC,
	/*  44 */ 0x007299,
	/*  45 */ 0x4C8599,
	/*  46 */ 0x005F7F,
	/*  47 */ 0x3F6F7F,
	/*  48 */ 0x00394C,
	/*  49 */ 0x26424C,
	/*  50 */ 0x00FFFF,
	/*  51 */ 0x7FFFFF,
	/*  52 */ 0x00CCCC,
	/*  53 */ 0x66CCCC,
	/*  54 */ 0x009999,
	/*  55 */ 0x4C9999,
	/*  56 */ 0x007F7F,
	/*  57 */ 0x3F7F7F,
	/*  58 */ 0x004C4C,
	/*  59 */ 0x264C4C,
	/*  60 */ 0x00FFBF,
	/*  61 */ 0x7FFFDF,
	/*  62 */ 0x00CC99,
	/*  63 */ 0x66CCB2,
	/*  64 */ 0x009972,
	/*  65 */ 0x4C9985,
	/*  66 */ 0x007F5F,
	/*  67 */ 0x3F7F6F,
	/*  68 */ 0x004C39,
	/*  69 */ 0x264C42,
	/*  70 */ 0x00FF7F,
	/*  71 */ 0x7FFFBF,
	/*  72 */ 0x00CC66,
	/*  73 */ 0x66CC99,
	/*  74 */ 0x00994C,
	/*  75 */ 0x4C9972,
	/*  76 */ 0x007F3F,
	/*  77 */ 0x3F7F5F,
	/*  78 */ 0x004C26,
	/*  79 */ 0x264C39,
	/*  80 */ 0x00FF3F,
	/*  81 */ 0x7FFF9F,
	/*  82 */ 0x00CC33,
	/*  83 */ 0x66CC7F,
	/*  84 */ 0x009926,
	/*  85 */ 0x4C995F,
	/*  86 */ 0x007F1F,
	/*  87 */ 0x3F7F4F,
	/*  88 */ 0x004C13,
	/*  89 */ 0x264C2F,
	/*  90 */ 0x00FF00,
	/*  91 */ 0x7FFF7F,
	/*  92 */ 0x00CC00,
	/*  93 */ 0x66CC66,
	/*  94 */ 0x009900,
	/*  95 */ 0x4C994C,
	/*  96 */ 0x007F00,
	/*  97 */ 0x3F7F3F,
	/*  98 */ 0x004C00,
	/*  99 */ 0x264C26,
	/* 100 */ 0x3FFF00,
	/* 101 */ 0x9FFF7F,
	/* 102 */ 0x33CC00,
	/* 103 */ 0x7FCC66,
	/* 104 */ 0x269900,
	/* 105 */ 0x5F994C,
	/* 106 */ 0x1F7F00,
	/* 107 */ 0x4F7F3F,
	/* 108 */ 0x134C00,
	/* 109 */ 0x2F4C26,
	/* 110 */ 0x7FFF00,
	/* 111 */ 0xBFFF7F,
	/* 112 */ 0x66CC00,
	/* 113 */ 0x99CC66,
	/* 114 */ 0x4C9900,
	/* 115 */ 0x72994C,
	/* 116 */ 0x3F7F00,
	/* 117 */ 0x5F7F3F,
	/* 118 */ 0x264C00,
	/* 119 */ 0x394C26,
	/* 120 */ 0xBFFF00,
	/* 121 */ 0xDFFF7F,
	/* 122 */ 0x99CC00,
	/* 123 */ 0xB2CC66,
	/* 124 */ 0x729900,
	/* 125 */ 0x85994C,
	/* 126 */ 0x5F7F00,
	/* 127 */ 0x6F7F3F,
	/* 128 */ 0x394C00,
	/* 129 */ 0x424C26,
	/* 130 */ 0xFFFF00,
	/* 131 */ 0xFFFF7F,
	/* 132 */ 0xCCCC00,
	/* 133 */ 0xCCCC66,
	/* 134 */ 0x999900,
	/* 135 */ 0x99994C,
	/* 136 */ 0x7F7F00,
	/* 137 */ 0x7F7F3F,
	/* 138 */ 0x4C4C00,
	/* 139 */ 0x4C4C26,
	/* 140 */ 0xFFBF00,
	/* 141 */ 0xFFDF7F,
	/* 142 */ 0xCC9900,
	/* 143 */ 0xCCB266,
	/* 144 */ 0x997200,
	/* 145 */ 0x99854C,
	/* 146 */ 0x7F5F00,
	/* 147 */ 0x7F6F3F,
	/* 148 */ 0x4C3900,
	/* 149 */ 0x4C4226,
	/* 150 */ 0xFF7F00,
	/* 151 */ 0xFFBF7F,
	/* 152 */ 0xCC6600,
	/* 153 */ 0xCC9966,
	/* 154 */ 0x994C00,
	/* 155 */ 0x99724C,
	/* 156 */ 0x7F3F00,
	/* 157 */ 0x7F5F3F,
	/* 158 */ 0x4C2600,
	/* 159 */ 0x4C3926,
	/* 160 */ 0xFF3F00,
	/* 161 */ 0xFF9F7F,
	/* 162 */ 0xCC3300,
	/* 163 */ 0xCC7F66,
	/* 164 */ 0x992600,
	/* 165 */ 0x995F4C,
	/* 166 */ 0x7F1F00,
	/* 167 */ 0x7F4F3F,
	/* 168 */ 0x4C1300,
	/* 169 */ 0x4C2F26,
	/* 170 */ 0xFF0000,
	/* 171 */ 0xFF7F7F,
	/* 172 */ 0xCC0000,
	/* 173 */ 0xCC6666,
	/* 174 */ 0x990000,
	/* 175 */ 0x994C4C,
	/* 176 */ 0x7F0000,
	/* 177 */ 0x7F3F3F,
	/* 178 */ 0x4C0000,
	/* 179 */ 0x4C2626,
	/* 180 */ 0xFF003F,
	/* 181 */ 0xFF7F9F,
	/* 182 */ 0xCC0033,
	/* 183 */ 0xCC667F,
	/* 184 */ 0x990026,
	/* 185 */ 0x994C5F,
	/* 186 */ 0x7F001F,
	/* 187 */ 0x7F3F4F,
	/* 188 */ 0x4C0013,
	/* 189 */ 0x4C262F,
	/* 190 */ 0xFF007F,
	/* 191 */ 0xFF7FBF,
	/* 192 */ 0xCC0066,
	/* 193 */ 0xCC6699,
	/* 194 */ 0x99004C,
	/* 195 */ 0x994C72,
	/* 196 */ 0x7F003F,
	/* 197 */ 0x7F3F5F,
	/* 198 */ 0x4C0026,
	/* 199 */ 0x4C2639,
	/* 200 */ 0xFF00BF,
	/* 201 */ 0xFF7FDF,
	/* 202 */ 0xCC0099,
	/* 203 */ 0xCC66B2,
	/* 204 */ 0x990072,
	/* 205 */ 0x994C85,
	/* 206 */ 0x7F005F,
	/* 207 */ 0x7F3F6F,
	/* 208 */ 0x4C0039,
	/* 209 */ 0x4C2642,
	/* 210 */ 0xFF00FF,
	/* 211 */ 0xFF7FFF,
	/* 212 */ 0xCC00CC,
	/* 213 */ 0xCC66CC,
	/* 214 */ 0x990099,
	/* 215 */ 0x994C99,
	/* 216 */ 0x7F007F,
	/* 217 */ 0x7F3F7F,
	/* 218 */ 0x4C004C,
	/* 219 */ 0x4C264C,
	/* 220 */ 0xBF00FF,
	/* 221 */ 0xDF7FFF,
	/* 222 */ 0x9900CC,
	/* 223 */ 0xB266CC,
	/* 224 */ 0x720099,
	/* 225 */ 0x854C99,
	/* 226 */ 0x5F007F,
	/* 227 */ 0x6F3F7F,
	/* 228 */ 0x39004C,
	/* 229 */ 0x42264C,
	/* 230 */ 0x7F00FF,
	/* 231 */ 0xBF7FFF,
	/* 232 */ 0x6600CC,
	/* 233 */ 0x9966CC,
	/* 234 */ 0x4C0099,
	/* 235 */ 0x724C99,
	/* 236 */ 0x3F007F,
	/* 237 */ 0x5F3F7F,
	/* 238 */ 0x26004C,
	/* 239 */ 0x39264C,
	/* 240 */ 0x3F00FF,
	/* 241 */ 0x9F7FFF,
	/* 242 */ 0x3300CC,
	/* 243 */ 0x7F66CC,
	/* 244 */ 0x260099,
	/* 245 */ 0x5F4C99,
	/* 246 */ 0x1F007F,
	/* 247 */ 0x4F3F7F,
	/* 248 */ 0x13004C,
	/* 249 */ 0x2F264C,

	// grays:
	/* 250 */ 0x333333,
	/* 251 */ 0x5b5b5b,
	/* 252 */ 0x848484,
	/* 253 */ 0xadadad,
	/* 254 */ 0xd6d6d6,
	/* 255 */ 0xffffff,
} ;

const size_t gNumColors = sizeof(gColors) / sizeof(*gColors);





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// globals:

#if 0
static inline void Rotate(double RotAng, double x, double y, double * Nx, double * Ny)
{
	RotAng = RotAng * PIDiv180;
	const double SinVal = std::sin(RotAng);
	const double CosVal = std::cos(RotAng);
	(*Nx) = x * CosVal - y * SinVal;
	(*Ny) = y * CosVal + x * SinVal;
}





static inline void Rotate(double RotAng, double x, double y, double ox, double oy, double * Nx, double * Ny)
{
	Rotate(RotAng, x - ox, y - oy, Nx, Ny);
	(*Nx) = (*Nx) + ox;
	(*Ny) = (*Ny) + oy;
}





constexpr uint32_t DXFColorToRGB(Color iColor)
{
	if ((iColor < 0) || (static_cast<size_t>(iColor) >= ARRAYCOUNT(gDXFColors)))
	{
		return RGB(0x00u, 0x00u, 0x00u);
	}

	return gDXFColors[static_cast<size_t>(iColor)];
}





double DXFGetRelativeTextWidth(const std::string & iText)
{
	// TODO: Proper width calculation
	return static_cast<double>(iText.size()) * 0.85;
}





/*
inline int DXFXToCanvasX(Viewport * Viewport, double x)
{
	return (Viewport->wid / 2) + ROUND((x - Viewport->wX)  / Viewport->Zoom);
}




inline int DXFYToCanvasY(Viewport * Viewport, double y)
{
	return (Viewport->hei / 2) + ROUND((Viewport->wY - y) / Viewport->Zoom);
}
*/





#define MAX_NAME_LENGTH 31u
#define MAX_CHANGED_LETTERS 9u  // Must be MAX_CHANGED_LETTERS <= MAX_NAME_LENGTH
#define DIGIT_COUNT static_cast<unsigned int>('9' - '0')





static void DXFFixName(std::string & Name)
{
	for (auto itr = Name.begin(), end = Name.end(); itr != end; ++itr)
	{
		if ((*itr >= 'a') && (*itr <= 'z'))
		{
			*itr = static_cast<char>(toupper(*itr));  // change to upper-case
		}
		else if ((*itr >= 'A') && (*itr <= 'Z'))
		{
			;  // keep it
		}
		else if ((*itr >= '0') && (*itr <= '9'))
		{
			;  // keep it
		}
		else
		{
			*itr = '_';
		}
	}  // for itr - Name[]

	// DXF in version lower than 2000 must have short layer/block names (up to 31 chars)
	if (Name.size() > MAX_NAME_LENGTH)
	{
		Name.erase(MAX_NAME_LENGTH);
	}
}





static bool IsShortNameUnique(const std::vector<std::string> & iNames, const std::string & iName)
{
	for (std::vector<std::string>::const_iterator i = iNames.begin(); i != iNames.end(); ++i)
	{
		if (CompareString(LOCALE_INVARIANT, NORM_IGNORECASE, i->c_str(), -1, iName.c_str(), -1) == CSTR_EQUAL)
		{
			return false;
		}
	}

	return true;
}





static bool FindUniqueShortName(const std::vector<std::string> & iNames, std::string & ioName)
{
	std::string name(ioName);
	for (size_t changed = 0u; changed < MAX_CHANGED_LETTERS; ++changed)
	{
		const size_t combinationCount = static_cast<size_t>(
			std::pow(static_cast<double>(DIGIT_COUNT), static_cast<int>(changed + 1u)));
		const size_t firstChanged = MAX_NAME_LENGTH - changed;

		for (size_t combination = 0u; combination < combinationCount; ++combination)
		{
			size_t bitArray = combination;
			for (size_t i = MAX_NAME_LENGTH; i >= firstChanged;)
			{
				--i;
				name[i] = static_cast<char>('0' + (bitArray % DIGIT_COUNT));
				bitArray /= DIGIT_COUNT;
			}

			if (IsShortNameUnique(iNames, name))
			{
				ioName = name;
				return true;
			}
		}
	}

	return false;  // No combination found, layers/blocks are merged
}





template <typename T>
static void DXFFixNames(T & iObjects)
{
	std::vector<std::string> shortNames;
	for (typename T::const_iterator it = iObjects.begin(); it != iObjects.end(); ++it)
	{
		std::string & name = (*it)->Name;
		// DXF in version lower than 2000 must have short layer/block names (up to 31 chars)
		if (name.size() <= MAX_NAME_LENGTH)
		{
			DXFFixName(name);
			shortNames.push_back(name);
		}
	}

	for (typename T::const_iterator it = iObjects.begin(); it != iObjects.end(); ++it)
	{
		std::string & name = (*it)->Name;
		if (name.size() <= MAX_NAME_LENGTH)
		{
			continue;
		}

		DXFFixName(name);
		if (IsShortNameUnique(shortNames, name) || FindUniqueShortName(shortNames, name))
		{
			shortNames.push_back(name);
		}
	}
}





std::string DXFFixLayerName(const std::string & iName)
{
	std::string res(iName);
	DXFFixName(res);
	return res;
}





void DXFFixLayerName(std::string & Name)
{
	DXFFixName(Name);
}





std::string DXFFixBlockName(const std::string & iName)
{
	std::string res(iName);
	DXFFixName(res);
	return res;
}





void DXFFixBlockName(std::string & Name)
{
	DXFFixName(Name);
}





void DXFFixLayerNames(Layers & iLayers)
{
	DXFFixNames<Layers>(iLayers);
}





void DXFFixBlockNames(BlockDefinitions & iBlocks)
{
	DXFFixNames<BlockDefinitions>(iBlocks);
}





static void inline DXFWriteStream(TMStream * iStream, int iCode, const std::string & iVal)
{
	DXFWriteStream(iStream, iCode, iVal.c_str());
}





static void inline DXFWriteStream(TMStream * iStream, int iCode, const TCHAR * iVal)
{
	SF_ASSERT(iVal != NULL);

	std::string out;
	AppendPrintf(out, TEXT("%d\r\n%s"), iCode, iVal);
	// TODO: UNICODE support?
	iStream->WriteLn(out.c_str());
}





static void inline DXFWriteStream(TMStream * iStream, int iCode, int iVal)
{
	AString txt;
	AppendPrintf(txt, "%d\x0d\x0a%d", iCode, iVal);
	iStream->WriteLn(txt.c_str());
}





static void DXFWriteStreamColor(TMStream * iStream, Color iColor)
{
	if (iColor == DXF_COLOR_BYLAYER)
	{
		return;  // no value must be written in DXF
	}
	else if (iColor == DXF_COLOR_BYBLOCK)
	{
		iStream->WriteLn("62\r\n0");  // 62, 0
		return;
	}
	else if (iColor < 0)
	{
		// invalid color number, use dummy
		iStream->WriteLn("62\r\n0");  // 62, 0
		return;
	}
	else if (iColor < 256)
	{
		AString txt;
		AppendPrintf(txt, "62\r\n%d", iColor);
		iStream->WriteLn(txt.c_str());
	}
}





static void inline DXFWriteStream(TMStream * iStream, int iCode, double iVal)
{
	AString txt;
	AppendPrintf(txt, "%d\x0d\x0a%f", iCode, iVal);
	iStream->WriteLn(txt.c_str());
}





void DXF_FillColorCombo(HWND iCombo, Color iCurrentColor, LPCTSTR iTextByLayer, LPCTSTR iTextByBlock, LPCTSTR iTextGeneric)
{
	// Fills colors into the combobox specified
	// NOTE: if changing the ITEMDATA format, the same change must be propagated to DXF_GetColorComboVal() and DXF_DrawColorComboItem()!

	SendMessage(iCombo, CB_RESETCONTENT, 0u, 0);
	LRESULT idx = SendMessage(iCombo, CB_ADDSTRING, 0u, reinterpret_cast<LPARAM>(iTextByLayer));
	if ((idx == CB_ERR) || (idx == CB_ERRSPACE))
	{
		return;
	}

	SendMessage(iCombo, CB_SETITEMDATA, static_cast<size_t>(idx), gNumDXFColors + 1u);  // combo-boxes cannot store negative values in ItemData!
	if (iCurrentColor == DXF_COLOR_BYLAYER)
	{
		SendMessage(iCombo, CB_SETCURSEL, static_cast<size_t>(idx), 0);
	}
	idx = SendMessage(iCombo, CB_ADDSTRING, 0u, reinterpret_cast<LPARAM>(iTextByBlock));
	SendMessage(iCombo, CB_SETITEMDATA, static_cast<size_t>(idx), gNumDXFColors + 2u);
	if (iCurrentColor == DXF_COLOR_BYBLOCK)
	{
		SendMessage(iCombo, CB_SETCURSEL, static_cast<size_t>(idx), 0);
	}
	for (size_t i = 0; i < gNumDXFColors; ++i)
	{
		std::string Caption;
		AppendPrintf(Caption, iTextGeneric, i);
		idx = SendMessage(iCombo, CB_ADDSTRING, 0u, reinterpret_cast<LPARAM>(Caption.c_str()));
		SendMessage(iCombo, CB_SETITEMDATA, static_cast<WPARAM>(idx), static_cast<LPARAM>(i + 1u));
		if (static_cast<size_t>(iCurrentColor) == i)
		{
			SendMessage(iCombo, CB_SETCURSEL, static_cast<WPARAM>(idx), 0);
		}
	}
}





Color DXF_GetColorComboVal(HWND iCombo)
{
	// Retrieves color chosen inside a combo previously filled with DXF_FillColorCombo()
	// NOTE: if changing the ITEMDATA format, the same change must be propagated to DXF_FillColorCombo() and DXF_DrawColorComboItem()!

	const LRESULT idx = SendMessage(iCombo, CB_GETCURSEL, 0u, 0);
	if (idx == CB_ERR)
	{
		return DXF_COLOR_BYLAYER;
	}

	Color DXFColor = static_cast<Color>(SendMessage(iCombo, CB_GETITEMDATA, static_cast<size_t>(idx), 0));
	if (DXFColor > static_cast<int>(gNumDXFColors))
	{
		if (DXFColor == static_cast<int>(gNumDXFColors + 1u))
		{
			DXFColor = DXF_COLOR_BYBLOCK;
		}
		else
		{
			DXFColor = DXF_COLOR_BYLAYER;
		}
	}
	else
	{
		--DXFColor;
	}

	return DXFColor;
}





extern void DXF_DrawColorComboItem(HWND iCombo, LPDRAWITEMSTRUCT dis)
{
	// Processed from inside WM_DRAWITEM
	// NOTE: if changing the ITEMDATA format, the same change must be propagated to DXF_FillColorCombo() and DXF_GetColorComboVal()!

	int BackColor;
	COLORREF TextColor;
	if (dis->itemState & ODS_SELECTED)
	{
		BackColor = COLOR_HIGHLIGHT;
		TextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
	}
	else
	{
		BackColor = COLOR_WINDOW;
		TextColor = GetSysColor(COLOR_WINDOWTEXT);
	}
	HBRUSH br = GetSysColorBrush(BackColor);
	TBrushUnique brushGuard(br);
	FillRect(dis->hDC, &(dis->rcItem), br);

	const std::string & caption = GetComboBoxItemText(iCombo, dis->itemID);
	const COLORREF c = (dis->itemData > gNumDXFColors) ? RGB(0x00u, 0x00u, 0x00u)
		: DXFColorToRGB(static_cast<Color>(dis->itemData - 1u));

	br = CreateSolidBrush(c);
	brushGuard.Reset(br);
	RECT rc;
	rc.top = dis->rcItem.top + 1;
	rc.left = dis->rcItem.left + 2;
	rc.right = rc.left + 16;
	rc.bottom = dis->rcItem.bottom - 1;
	FillRect(dis->hDC, &rc, br);
	FrameRect(dis->hDC, &rc, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

	SetTextColor(dis->hDC, TextColor);
	SetTextAlign(dis->hDC, TA_TOP | TA_LEFT);
	SetBkMode(dis->hDC, TRANSPARENT);
	TextOut(dis->hDC, dis->rcItem.left + 20, dis->rcItem.top + 1, caption.c_str(), static_cast<int>(caption.size()));
}

#endif  // 0





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primitive

Primitive::Primitive(ObjectType aObjectType):
	mObjectType(aObjectType),
	mPos({0, 0}),
	mColor(COLOR_BYLAYER),
	mWidth(0)
{
}





Primitive::Primitive(ObjectType aObjectType, Coords && aPos, Color aColor, Coord aWidth):
	mObjectType(aObjectType),
	mPos(std::move(aPos)),
	mColor(aColor),
	mWidth(aWidth)
{
}





Extent Primitive::extent() const
{
	return {mPos, mPos};
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Attrib:

Attrib::Attrib(const std::string & aName, const std::string & aValue, Coord && aFontSize):
	mName(aName),
	mValue(aValue),
	mFontSize(aFontSize)
{
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ExtendedData:

ExtendedData::ExtendedData():
	mString(),
	mApplicationName(),
	mControlString(),
	mLayerName(),
	mBinaryData(),
	mDatabaseHandle(0),
	mPos(0, 0),
	mWorldSpacePosition(0, 0, 0),
	mWorldSpaceDisplacement(0, 0, 0),
	mWorldDirection(0, 0, 0),
	mReal(0),
	mDistance(0),
	mScaleFactor(0),
	mInteger(0),
	mLong(0)
{
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AxisAligned2DEllipse:

AxisAligned2DEllipse::AxisAligned2DEllipse(Coords && aPos, Coord aDiameterX, Coord aDiameterY):
	Primitive(otSimpleEllipse, std::move(aPos)),
	mDiameterX(aDiameterX),
	mDiameterY(aDiameterY)
{
}





Extent AxisAligned2DEllipse::extent() const
{
	return {mPos - Coords(mDiameterX, mDiameterY), mPos + Coords(mDiameterX, mDiameterY)};
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Line:

Line::Line(Coords && aPos1, Coords && aPos2, Color aColor, int aStyle, Coord && aWidth):
	Primitive(otLine, std::move(aPos1), aColor, aWidth),
	mPos2(std::move(aPos2)),
	mStyle(aStyle)
{
}





Extent Line::extent() const
{
	Extent res(mPos);
	res.expandTo(mPos2);
	return res;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Text:

Text::Text(Coords && aPos, const std::string & aRawText, Coord && aSize, Coord && aAngle, Color aColor, int aAlignment):
	Super(otText, std::move(aPos), std::move(aColor)),
	mRawText(aRawText),
	mAngle(std::move(aAngle)),
	mSize(std::move(aSize)),
	mOblique(0.0),
	mAlignment(aAlignment),
	mThickness(0.0)
{
	assert(mSize > 1e-10);
	assert(mSize < 1e12);
}





Text::Text(Coords && aPos, std::string && aRawText, Coord && aSize, Coord && aAngle, Color aColor, int aAlignment):
	Super(otText, std::move(aPos), std::move(aColor)),
	mRawText(std::move(aRawText)),
	mAngle(std::move(aAngle)),
	mSize(std::move(aSize)),
	mOblique(0.0),
	mAlignment(aAlignment),
	mThickness(0.0)
{
	assert(mSize > 1e-10);
	assert(mSize < 1e12);
}





Extent Text::extent() const
{
	// TODO: Better text length approximation
	// TODO: Strip formatting information before asking for length
	auto textWidth = mSize * mRawText.length();

	// TODO: Extent should follow the text alignment
	return {mPos - Coords(textWidth / 2, mSize), mPos + Coords(textWidth / 2, mSize)};
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Block:

Block::Block(Coords && aPos, std::shared_ptr<BlockDefinition> && aDefinition, Coord && aAngle, Coord aScaleMaster):
	Super(otBlock, std::move(aPos)),
	mDefinition(std::move(aDefinition)),
	mAngle(aAngle),
	mScale(aScaleMaster, aScaleMaster, aScaleMaster)
{
}





Extent Block::extent() const
{
	// TODO
	return {mPos, mPos};
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BlockDefinition:

BlockDefinition::BlockDefinition(const std::string & aName):
	mName(aName)
{
}





BlockDefinition::BlockDefinition(std::string && aName):
	mName(std::move(aName))
{
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MultiVertex:

void MultiVertex::addVertex(Coords && aCoords)
{
	mVertices.push_back(Vertex(std::move(aCoords)));
}





void MultiVertex::addVertex(Vertex && aVertex)
{
	mVertices.push_back(std::move(aVertex));
}





void MultiVertex::removeDuplicateVertices()
{
	if (mVertices.size() < 2)
	{
		return;
	}
	std::vector<Vertex> newVertices;
	for (size_t idx = mVertices.size() - 1; idx > 0; --idx)
	{
		if (mVertices[idx - 1].mPos == mVertices[idx].mPos)
		{
			mVertices.erase(mVertices.begin() + idx);
		}
	}
}





Extent MultiVertex::extent() const
{
	Extent res(mVertices[0].mPos);
	for (const auto & v: mVertices)
	{
		res.expandTo(v.mPos);
	}
	return res;
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Solid:

Solid::Solid(Coords && aPos1, Coords && aPos2, Coords && aPos3, Color aColor):
	Super(otSolid, std::move(aPos1), aColor),
	mPos2(std::move(aPos2)),
	mPos3(std::move(aPos3)),
	mPos4(0, 0),
	mIsTetra(false)
{
}





Solid::Solid(Coords && aPos1, Coords && aPos2, Coords && aPos3, Coords && aPos4, Color aColor):
	Super(otSolid, std::move(aPos1), aColor),
	mPos2(std::move(aPos2)),
	mPos3(std::move(aPos3)),
	mPos4(std::move(aPos4)),
	mIsTetra(true)
{
}





Extent Solid::extent() const
{
	Extent res(mPos);
	res.expandTo(mPos2);
	res.expandTo(mPos3);
	if (mIsTetra)
	{
		res.expandTo(mPos4);
	}
	return res;
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Circle:

Circle::Circle():
	Super(otCircle),
	mRadius(0)
{
}





Circle::Circle(Coords && aPos, Coord && aRadius, Color aColor):
	Super(otCircle, std::move(aPos), aColor),
	mRadius(aRadius)
{
}





Extent Circle::extent() const
{
	return {mPos - Coords(mRadius, mRadius, 0), mPos + Coords(mRadius, mRadius, 0)};
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arc:

Arc::Arc():
	Super(otArc),
	mRadius(0),
	mStartAngle(0),
	mEndAngle(0)
{
}





Arc::Arc(Coords && aCenterPos, Coord && aRadius, Coord && aStartAngle, Coord  && aEndAngle, Color aColor):
	Super(otArc, std::move(aCenterPos), aColor),
	mRadius(std::move(aRadius)),
	mStartAngle(std::move(aStartAngle)),
	mEndAngle(std::move(aEndAngle))
{
}





Extent Arc::extent() const
{
	// TODO: Proper extent, using the endpoint angles
	return {mPos - Coords(mRadius, mRadius), mPos + Coords(mRadius, mRadius)};
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Layer:

Layer::Layer(Drawing & aParentDrawing, const std::string & aName):
	mParentDrawing(aParentDrawing),
	mDefaultColor(COLOR_BYLAYER),  // dummy, forces black color
	mName(aName)
{
}





void Layer::clear()
{
	mObjects.clear();
	mExtent = Extent();
}





void Layer::updateExtent()
{
	Extent extent;
	for (const auto & obj: mObjects)
	{
		extent.expandTo(obj->extent());
	}
	mExtent = extent;
}





PrimitivePtr Layer::removeObjByIndex(size_t aIndex)
{
	auto res = mObjects[aIndex];
	mObjects.erase(mObjects.begin() + aIndex);
	return res;
}





void Layer::removeObj(Primitive * aObject)
{
	auto cmp = [aObject](const PrimitivePtr aStoredObj){ return (aStoredObj.get() == aObject); };
	mObjects.erase(std::remove_if(mObjects.begin(), mObjects.end(), cmp), mObjects.end());
}





void Layer::addObject(PrimitivePtr && aObject)
{
	mObjects.push_back(std::move(aObject));
}





void Layer::addObject(const PrimitivePtr & aObject)
{
	mObjects.push_back(aObject);
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Drawing:

void Drawing::clear()
{
	mLayers.clear();
	mBlockDefinitions.clear();
}





std::shared_ptr<Layer> Drawing::addLayer(const std::string & aName)
{
	assert(!aName.empty());

	// Check that the layer doesn't exist already:
	auto lay = layerByName(aName);
	if (lay != nullptr)
	{
		throw LayerAlreadyExists(aName);
	}

	mLayers.push_back(std::make_shared<Layer>(*this, aName));
	return mLayers.back();
}






std::shared_ptr<Layer> Drawing::layerByName(const std::string & aName) const
{
	for (const auto & lay: mLayers)
	{
		if (lay->name() == aName)
		{
			return lay;
		}
	}
	return nullptr;
}





void Drawing::addBlockDefinition(std::string && aName, std::shared_ptr<BlockDefinition> aBlockDefinition)
{
	assert(aBlockDefinition->mName == aName);

	// Check that the block definition is not already present:
	if (mBlockDefinitions.find(aName) != mBlockDefinitions.end())
	{
		throw BlockDefinitionAlreadyExists(aName);
	}

	// Store the block definition:
	mBlockDefinitions[std::move(aName)] = aBlockDefinition;
}





}  // namespace Dxf
