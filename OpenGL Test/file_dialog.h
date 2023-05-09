#pragma 
#include <windows.h>      // For common windows data types and function headers
#define STRICT_TYPED_ITEMIDS
#include <shlobj.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <shlwapi.h>
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC
#include <new>

const COMDLG_FILTERSPEC allowed_file_types[] = {
	{L"Glow Save File (*.glowsave)", L"*.glowsave"}
};

class CDialogEventHandler : public IFileDialogEvents,
	public IFileDialogControlEvents
{
public:
	// IUnknown methods
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = {
			QITABENT(CDialogEventHandler, IFileDialogEvents),
			QITABENT(CDialogEventHandler, IFileDialogControlEvents),
			{ 0 }
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release()
	{
		long cRef = InterlockedDecrement(&_cRef);
		if (!cRef)
			delete this;
		return cRef;
	}

	// IFileDialogEvents methods
	IFACEMETHODIMP OnFileOk(IFileDialog*) {
		return S_OK;
	};
	IFACEMETHODIMP OnFolderChange(IFileDialog*) {
		return S_OK;
	};
	IFACEMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) {
		return S_OK;
	};
	IFACEMETHODIMP OnHelp(IFileDialog*) {
		return S_OK;
	};
	IFACEMETHODIMP OnSelectionChange(IFileDialog*) {
		return S_OK;
	};
	IFACEMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) {
		return S_OK;
	};
	IFACEMETHODIMP OnTypeChange(IFileDialog* pfd) {
		return S_OK;
	};
	IFACEMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) {
		return S_OK;
	};

	// IFileDialogControlEvents methods
	IFACEMETHODIMP OnItemSelected(IFileDialogCustomize* pfdc, DWORD dwIDCtl, DWORD dwIDItem) {
		return S_OK;
	};
	IFACEMETHODIMP OnButtonClicked(IFileDialogCustomize*, DWORD) {
		return S_OK;
	};
	IFACEMETHODIMP OnCheckButtonToggled(IFileDialogCustomize*, DWORD, BOOL) {
		return S_OK;
	};
	IFACEMETHODIMP OnControlActivating(IFileDialogCustomize*, DWORD) {
		return S_OK;
	};

	CDialogEventHandler() : _cRef(1) { };
private:
	~CDialogEventHandler() { };
	long _cRef;
};


// Instance creation helper
HRESULT CDialogEventHandler_CreateInstance(REFIID riid, void** ppv)
{
	*ppv = NULL;
	CDialogEventHandler* pDialogEventHandler = new (std::nothrow) CDialogEventHandler();
	HRESULT hr = pDialogEventHandler ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr))
	{
		hr = pDialogEventHandler->QueryInterface(riid, ppv);
		pDialogEventHandler->Release();
	}
	return hr;
}

HRESULT ImportDialog(char* filePath)
{
	// CoCreate the File Open Dialog object.
	IFileDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr))
	{
		// Create an event handling object, and hook it up to the dialog.
		IFileDialogEvents* pfde = NULL;
		hr = CDialogEventHandler_CreateInstance(IID_PPV_ARGS(&pfde));
		if (SUCCEEDED(hr))
		{
			// Hook up the event handler.
			DWORD dwCookie;
			hr = pfd->Advise(pfde, &dwCookie);
			if (SUCCEEDED(hr))
			{
				// Set the options on the dialog.
				DWORD dwFlags;

				// Before setting, always get the options first in order not to override existing options.
				hr = pfd->GetOptions(&dwFlags);
				if (SUCCEEDED(hr))
				{
					// In this case, get shell items only for file system items.
					hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);
					if (SUCCEEDED(hr))
					{
						IKnownFolderManager* pkfm = NULL;
						hr = CoCreateInstance(CLSID_KnownFolderManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pkfm));
						if (SUCCEEDED(hr))
						{
							// Get the known folder.
							IKnownFolder* pKnownFolder = NULL;
							hr = pkfm->GetFolder(FOLDERID_Downloads, &pKnownFolder);
							if (SUCCEEDED(hr))
							{
								// File Dialog APIs need an IShellItem that represents the location.
								IShellItem* psi = NULL;
								hr = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
								if (SUCCEEDED(hr))
								{
									hr = pfd->SetFolder(psi);
									if (SUCCEEDED(hr))
									{
										// Set the file types to display only. Notice that, this is a 1-based array.
										hr = pfd->SetFileTypes(1, allowed_file_types);
										if (SUCCEEDED(hr))
										{
											pfd->SetDefaultExtension(L"glowsave");
											pfd->SetTitle(L"Import Save");
											pfd->SetOkButtonLabel(L"Import");
											// Show the dialog
											hr = pfd->Show(NULL);
											if (SUCCEEDED(hr))
											{
												// Obtain the result, once the user clicks the 'Open' button.
												// The result is an IShellItem object.
												IShellItem* psiResult;
												hr = pfd->GetResult(&psiResult);
												if (SUCCEEDED(hr))
												{
													PWSTR wFilePath = NULL;
													hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &wFilePath);
													if (wFilePath == NULL) return -1;
													wcstombs(filePath, wFilePath, MAX_PATH);
													CoTaskMemFree(wFilePath);
													psiResult->Release();
												}
											}
										}
									}
									psi->Release();
								}
								pKnownFolder->Release();
							}
							pkfm->Release();
						}
					}
				}
				pfd->Unadvise(dwCookie);
			}
			pfde->Release();
		}
		pfd->Release();
	}
	return hr;
}

HRESULT ExportDialog(char* filePath, const char* save_name)
{
	// CoCreate the File Open Dialog object.
	IFileSaveDialog* pfd = NULL;
	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

	if (SUCCEEDED(hr))
	{
		// Set the file types to display only. Notice that, this is a 1-based array.
		hr = pfd->SetFileTypes(1, allowed_file_types);
		if (SUCCEEDED(hr))
		{
			hr = pfd->SetDefaultExtension(L"glowsave");
			hr = pfd->SetTitle(L"Export Save");
			hr = pfd->SetOkButtonLabel(L"Export");

			// Set default file name
			wchar_t wSaveName[21];

			mbstowcs(wSaveName, save_name, 21);
			if (wSaveName == NULL) return -1;

			hr = pfd->SetFileName(wSaveName);
			if (SUCCEEDED(hr))
			{
				// Show the dialog
				hr = pfd->Show(NULL);
				if (SUCCEEDED(hr))
				{
					// Obtain the result, once the user clicks the 'Open' button.
					// The result is an IShellItem object.
					IShellItem* psiResult;
					hr = pfd->GetResult(&psiResult);
					if (SUCCEEDED(hr))
					{
						PWSTR wFilePath = NULL;
						hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &wFilePath);
						if (wFilePath == NULL) return -1;
						wcstombs(filePath, wFilePath, MAX_PATH);
						CoTaskMemFree(wFilePath);
						psiResult->Release();
					}
				}
			}
		}
		pfd->Release();
	}
	return hr;
}