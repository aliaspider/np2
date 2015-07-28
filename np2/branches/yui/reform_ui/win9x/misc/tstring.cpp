/**
 * @file	tstring.cpp
 * @brief	文字列クラスの動作の定義を行います
 */

#include "compiler.h"
#include "tstring.h"
#include "WndProc.h"

/**
 * リソースから文字列をロード
 * @param[in] uID 文字列ID
 * @return 文字列
 */
std::tstring LoadTString(UINT uID)
{
	std::tstring ret;

	do
	{
		HMODULE hModule = static_cast<HMODULE>(CWndProc::GetResourceHandle());

		HRSRC hRsrc = ::FindResource(hModule, MAKEINTRESOURCE((uID >> 4) + 1), RT_STRING);
		if (hRsrc == NULL)
		{
			break;
		}

		DWORD dwResSize = ::SizeofResource(hModule, hRsrc);
		HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
		if (hGlobal == NULL)
		{
			break;
		}

		LPCWSTR pRes = static_cast<LPCWSTR>(::LockResource(hGlobal));
		DWORD dwPos = 0;
		uID = uID & 15;
		while ((uID) && (dwPos < dwResSize))
		{
			dwPos += pRes[dwPos] + 1;
			uID--;
		}
		if (dwPos >= dwResSize)
		{
			break;
		}

		UINT nLength = pRes[dwPos++];
		nLength = min(nLength, (dwResSize - dwPos));

#if defined(_UNICODE)

		ret = std::wstring(pRes + dwPos, nLength);

#else	// defined(_UNICODE)

		UINT nSize = ::WideCharToMultiByte(CP_ACP, 0, pRes + dwPos, nLength, NULL, 0, NULL, NULL);
		if (nSize)
		{
			ret.resize(nSize);
			nSize = ::WideCharToMultiByte(CP_ACP, 0, pRes + dwPos, nLength, &ret.at(0), nSize, NULL, NULL);
			ret.resize(nSize);
		}

#endif	// defined(_UNICODE)

	} while (false /*CONSTCOND*/);

	return ret;
}
