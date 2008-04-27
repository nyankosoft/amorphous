


/* ================================== 参考 ==================================

	singleton	: singleton template
		programmed & desinged by yaneurao(M.Isozaki) '02/03/13
*/

#ifndef __YTL_SINGLETON_H__
#define __YTL_SINGLETON_H__

/*
#ifdef USE_MEMORY_STATE
	#include "../Auxiliary/yaneMemory.h"
#endif

#include "../Thread/yaneCriticalSection.h"

namespace yaneuraoGameSDK3rd {
namespace YTL {
*/


#include <stdlib.h>

namespace NS_KGL
{


template <class T>
class CSingleton
{

/**
	このポインタは、
	必要に迫られたときに始めてobjectをnewするようなポインタ

	非ローカルなstaticオブジェクトとして、

	class CHoge{
	public:
		static singleton<CHoge> m_obj;
		static CHoge* GetObj() { return m_obj->get(); }
	};
	のように使って大丈夫！オブジェクトの解体についても保証される

	というか、このクラスは、ひとつのクラスにつき、一つしか存在できない。
	おまけに、上記のようにstaticとしてしか使えない(;´Д`)
*/
public:

	CSingleton()
	{
	//	初期化は何も行なわない
	//	というか非ローカルなstaticオブジェクトの
	//	初期化の問題があるので、このオブジェクトの初期化については
	//	何も行なってはいけない
	}

	~CSingleton() { Release(); }

	///	ポインタのふりをするための仕掛け
	T& operator*() { return *get(); }
	T* operator->() { return get();	}
	T* get() { CheckInstance(); return m_lpObj; }

	///	オブジェクトがNullかどうかを検証する
	bool	isNull() const { return m_lpObj == NULL; }

	///	オブジェクトを強制的に解体する
	///	（デストラクタでも解放している）
	void Release();

	void CheckInstance();
	/**
		インスタンスが存在するのかそのチェックを行ない、
		存在しなければ生成する。本来、明示的に行なう必要は無いが、
		このオブジェクト自体が非常に初期化の時間のかかり、
		リアルタイムにその初期化を行なわれては困るときには、
		（必要なのがわかっていれば）事前に生成を行なっておく
		ことで、それを回避できる。
	*/

#if 0
//#ifdef USE_MEMORY_STATE
		if (CMemoryState::IsActive()){
		/**	Log記録中(BeginSnap～EndSnapの最中)
			このときに生成されたオブジェクトは、EndSnapのあと、
			operator new/deleteのオーバーロードが解除されるまでに
			オブジェクトをコールバックによって解体してやる必要がある
		*/
			//	callbackオブジェクトを用意
			smart_ptr<function_callback> fn(
				function_callback_v::Create((void(singleton<T>::*)())Release,this)
			);
			CMemoryState::RegistCallBack(fn);
			//	callbackを依頼する
		}
//#endif
#endif

protected:
//	static CCriticalSection m_cs;
	//	↑これは、シングルスレッド時は、用いない

	static T*	m_lpObj;
	//	staticで用意することで、
	//	リンク時(非ローカルなstaticオブジェクトの初期化前)にNULL
	//	であることを保証する
};

///////////////////////////////////////////////////////////////////////////////////////

//	static なオブジェクト
template <class T> T* CSingleton<T>::m_lpObj = 0;

//template <class T> CCriticalSection CSingleton<T>::m_cs;

//}}

//#include "../AppFrame/yaneAppInitializer.h"	//	マルチスレッド動作かどうかを取得

//namespace yaneuraoGameSDK3rd {
//namespace YTL {


//	↓この実装で必要となるCAppInitializerのなかでsingeltonを用いているので↑で
//		includeしないと、循環参照になってしまう
template <class T> void	CSingleton<T>::CheckInstance()
{
	if (m_lpObj==NULL)
	{
/*		if (CAppInitializer::IsMultiThread())
		{
		//	マルチスレッド時は、double-checked locking
			CCriticalLock guard(&m_cs);
			if (m_lpObj==NULL)
			{
				m_lpObj = new T;
			}
		}
		else*/
		{
			m_lpObj = new T;
		}
	}
}


template <class T> void	CSingleton<T>::Release()
{
	if (m_lpObj!=NULL)
	{
/*		if (CAppInitializer::IsMultiThread())
		{
		//	マルチスレッド時は、double-checked locking
			CCriticalLock guard(&m_cs);
			if (m_lpObj!=NULL)
			{
				delete m_lpObj;
				m_lpObj = NULL;
			}
		}
		else*/
		{
			delete m_lpObj;
			m_lpObj = NULL;
		}
	}
}


}	/*  NS_KGL  */


/*
} // end of namespace YTL
} // end of namespace yaneuraoGameSDK3rd
*/

#endif  /*  __YTL_SINGLETON_H__  */
