#ifndef __WETLA_HOST_H__
#define __WETLA_HOST_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
	#error wetla_host.h requires atlapp.h to be included first
#endif

#ifndef __ATLHOST_H__
	#error wetla_host.h requires atlhost.h to be included first
#endif


namespace WETLA
{

//Helper for advising connections points from a sink map
template <class T>
inline HRESULT AdviseDispEventMap(T* pT, UINT nID, IUnknown* pUnk, bool bAdvise)
{
	const _ATL_EVENT_ENTRY<T>* pEntries = T::_GetSinkMap();
	if (pUnk == NULL || pEntries == NULL)
		return S_OK;

	HRESULT hr = S_OK;
	while (pEntries->piid != NULL)
	{
		_IDispEvent* pDE = (_IDispEvent*)((DWORD_PTR)pT + pEntries->nOffset);
		bool bNotAdvised = pDE->m_dwEventCookie == 0xFEFEFEFE;

		if (bAdvise ^ bNotAdvised || pEntries->nControlID != nID)
		{
			pEntries++;
			continue;
		}

		if (bAdvise)
		{
			if (!InlineIsEqualGUID(IID_NULL, *pEntries->piid))
				hr = pDE->DispEventAdvise(pUnk, pEntries->piid);
			else
			{
				AtlGetObjectSourceInterface(pUnk, &pDE->m_libid, &pDE->m_iid,
					&pDE->m_wMajorVerNum, &pDE->m_wMinorVerNum);
				hr = pDE->DispEventAdvise(pUnk, &pDE->m_iid);
			}
		}
		else
		{
			if (!InlineIsEqualGUID(IID_NULL, *pEntries->piid))
				hr = pDE->DispEventUnadvise(pUnk, pEntries->piid);
			else
				hr = pDE->DispEventUnadvise(pUnk, &pDE->m_iid);
		}
		ATLASSERT(hr == S_OK);

		if (FAILED(hr))
			break;
		pEntries++;
	}
	return hr;
}

}; //namespace WETLA

#endif // __WETLA_HOST_H__
