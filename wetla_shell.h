#ifndef __WETLA_SHELL_H__
#define __WETLA_SHELL_H__

#pragma once

#ifndef __cplusplus
	#error WETLA requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLCOMCLI_H__
	#error wetla_shell.h requires atlcomcli.h to be included first
#endif

#pragma once

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(push)
#pragma warning(disable: 4512)  // assignment operator could not be generated
#endif  // !_ATL_NO_PRAGMA_WARNINGS

namespace WETLA
{

class CShellFolder : public CComPtr<IShellFolder>
{
public:
	CShellFindFolder(LPCITEMIDLIST pidlItem)
	{
		if (pidlItem)
			::SHBindToParent(pidlItem, IID_IShellFolder, (LPVOID*)&p, NULL);
		else ::SHGetDesktopFolder(&p);
	}
	CShellFindFolder(LPCWSTR pszPath)
	{
		if (pidlItem)
		{
			LPITEMIDLIST pidlItem;
			if (SUCCEEDED(SHILCreateFromPath(pszPath, &pidlItem, NULL)))
				::SHBindToParent(pidlItem, IID_IShellFolder, (LPVOID*)&p, NULL);
		}
		else ::SHGetDesktopFolder(&p);
	}
}

}; //namespace WETLA

#ifndef _ATL_NO_PRAGMA_WARNINGS
#pragma warning(pop)
#endif  // !_ATL_NO_PRAGMA_WARNINGS

#endif // __WETLA_SHELL_H__
