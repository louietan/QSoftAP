#include "network/ics_service.h"

#include <NetCon.h>
#include "network/ics_connection.h"

namespace network
{

using network::IcsConnection;

IcsService::IcsService()
{
	CoInitialize (nullptr);
	CoInitializeSecurity (nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_PKT,
						  RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
	CoCreateInstance (__uuidof (NetSharingManager), nullptr, CLSCTX_ALL,
					  __uuidof (INetSharingManager),
					  (LPVOID*)&this->sharing_manager_);

	this->reloadConnections();
}

void IcsService::reloadConnections()
{
	HRESULT  hr = S_OK;
	this->connections.clear();

	INetSharingEveryConnectionCollection* collect = nullptr;
	hr = this->sharing_manager_->get_EnumEveryConnection (&collect);

	if (hr == S_OK) {
		/// 摘自网络，稍做修改
		INetConnection* pNetConnect = nullptr;  // fill this out for part 2 below
		// enumerate connections
		IEnumVARIANT* pEV = nullptr;
		IUnknown* pUnk = nullptr;

		if (collect != nullptr) {
			collect->get__NewEnum (&pUnk);

			if (pUnk) {
				pUnk->QueryInterface (__uuidof (IEnumVARIANT), (void**)&pEV);
				pUnk->Release();
			}

			if (pEV) {
				VARIANT variant;
				VariantInit (&variant);

				//INetSharingConfiguration *pNSC = NULL;
				while (S_OK == pEV->Next (1, &variant, nullptr)) {
					if (V_VT (&variant) == VT_UNKNOWN) {
						V_UNKNOWN (&variant)
						->QueryInterface (__uuidof (INetConnection), (void**)&pNetConnect);

						if (pNetConnect) {
							this->connections.push_back ({pNetConnect, this->sharing_manager_});
							pNetConnect->Release();
						}
					}

					VariantClear (&variant);
				}  // end while

				pEV->Release();
			}  // end if

			collect->Release();
		}
	}

	///

	emit reloadFinished (*this);
}

void IcsService::enableSharing (const std::string& pub, const std::string& pri)
{
	this->disableAll();

	for (auto& i : this->connections) {
		if (i.guid() == pub) {
			i.enableAsPublic();
		} else if (i.guid() == pri) {
			i.enableAsPrivate();
		}
	}
}

void IcsService::disableAll()
{
	for (auto i : this->connections) {
		if (i.sharingEnabled()) { i.config()->DisableSharing(); }
	}
}

bool IcsService::findById (const std::string& id, IcsConnection& result)
{
	for (auto& i : this->connections) {
		if (i.guid() == id) {
			result = i;
			return true;
		}
	}

	return false;
}

IcsService::~IcsService()
{
	this->sharing_manager_->Release();
	this->sharing_manager_ = nullptr;
}
}
