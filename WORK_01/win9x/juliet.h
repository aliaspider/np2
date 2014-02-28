/**
 * @file	juliet.h
 * @brief	ROMEO アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief ROMEO アクセス クラス
 */
class CJuliet
{
public:
	static CJuliet* GetInstance();
	CJuliet();
	bool Initialize();
	void Deinitialize();
	bool IsEnabled() const;
	bool IsBusy() const;
	void Reset() const;
	void WriteRegister(UINT nAddr, UINT8 cData);
	void Mute(bool bMute) const;
	void Restore(const UINT8* data, bool bOpna);

private:
	//! @brief ロード関数
	struct ProcItem
	{
		LPCSTR lpSymbol;		//!< 関数名
		size_t nOffset;			//!< オフセット
	};

	static CJuliet sm_instance;												//!< 唯一のインスタンスです

	HMODULE m_hModule;														//!< モジュール
	ULONG (WINAPI * m_fnRead32)(ULONG ulPciAddress, ULONG ulRegAddress);	//!< コンフィグレーション読み取り関数
	VOID (WINAPI * m_fnOut8)(ULONG ulAddress, UCHAR ucParam);				//!< outp 関数
	VOID (WINAPI * m_fnOut32)(ULONG ulAddress, ULONG ulParam);				//!< outpd 関数
	UCHAR (WINAPI * m_fnIn8)(ULONG ulAddress);								//!< inp 関数
	bool m_bOpna;															//!< OPNA 有効フラグ
	UCHAR m_ucIrq;															//!< ROMEO IRQ
	UINT8 m_cPsgMix;														//!< PSG ミキサー
	ULONG m_ulAddress;														//!< ROMEO ベース アドレス
	UINT8 m_cAlgorithm[8];													//!< アルゴリズム テーブル
	UINT8 m_cTtl[8 * 4];													//!< TTL テーブル

	void Clear();
	ULONG SearchRomeo() const;
	void WriteRegisterInner(UINT nAddr, UINT8 cData) const;
	void SetVolume(UINT nChannel, int nVolume) const;
};

/**
 * インスタンスを得る
 * @return インスタンス
 */
inline CJuliet* CJuliet::GetInstance()
{
	return &sm_instance;
}

/**
 * ROMEO は有効?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CJuliet::IsEnabled() const
{
	return (m_hModule != NULL);
}

#if 0
#if !defined(SUPPORT_ROMEO)

#define	juliet_initialize()		(FAILURE)
#define	juliet_deinitialize()

#define	juliet_YMF288Reset()
#define juliet_YMF288IsEnable()	(FALSE)
#define juliet_YMF288IsBusy()	(FALSE)
#define juliet_YMF288A(a, d)
#define juliet_YMF288B(a, d)
#define juliet_YMF288Enable(e)

#else

inline BOOL juliet_initialize(void)
{
	return CJuliet::GetInstance()->Initialize() ? SUCCESS : FAILURE;
}

inline void juliet_deinitialize(void)
{
	CJuliet::GetInstance()->Deinitialize();
}

inline void juliet_YMF288Reset(void)
{
	CJuliet::GetInstance()->Reset();
}

inline BOOL juliet_YMF288IsEnable(void)
{
	return CJuliet::GetInstance()->IsEnabled();
}

inline BOOL juliet_YMF288IsBusy(void)
{
	return CJuliet::GetInstance()->IsBusy();
}

inline void juliet_YMF288A(UINT addr, UINT8 data)
{
	CJuliet::GetInstance()->WriteRegister(addr, data);
}

inline void juliet_YMF288B(UINT addr, UINT8 data)
{
	CJuliet::GetInstance()->WriteRegister(addr + 0x100, data);
}

inline void juliet_YMF288Enable(BOOL enable)
{
	CJuliet::GetInstance()->Mute(!enable);
}

#endif
#endif
