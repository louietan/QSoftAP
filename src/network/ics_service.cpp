#include "network/ics_service.h"

#include <NetCon.h>
#include "network/ics_connection.h"

namespace network {

using network::IcsConnection;

IcsService::IcsService() {
  CoInitialize(nullptr);
  CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT,
                       RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
  CoCreateInstance(__uuidof(NetSharingManager), nullptr, CLSCTX_ALL,
                   __uuidof(INetSharingManager),
                   (LPVOID *)&this->sharing_manager_);

  this->reloadConnections();
}

void IcsService::reloadConnections() {
  this->connections.clear();

  INetSharingEveryConnectionCollection *collect = nullptr;
  this->sharing_manager_->get_EnumEveryConnection(&collect);

  /// Õª×ÔÍøÂç£¬ÉÔ×öÐÞ¸Ä
  INetConnection *pNetConnect = NULL;  // fill this out for part 2 below
  // enumerate connections
  IEnumVARIANT *pEV = NULL;
  IUnknown *pUnk = NULL;
  collect->get__NewEnum(&pUnk);
  if (pUnk) {
    pUnk->QueryInterface(__uuidof(IEnumVARIANT), (void **)&pEV);
    pUnk->Release();
  }
  if (pEV) {
    VARIANT variant;
    VariantInit(&variant);
    INetSharingConfiguration *pNSC = NULL;
    while (S_OK == pEV->Next(1, &variant, NULL)) {
      if (V_VT(&variant) == VT_UNKNOWN) {
        V_UNKNOWN(&variant)
            ->QueryInterface(__uuidof(INetConnection), (void **)&pNetConnect);
        if (pNetConnect) {
          this->connections.push_back({pNetConnect, this->sharing_manager_});
        }
      }
      VariantClear(&variant);
    }  // end while
    pEV->Release();
  }  // end if
  collect->Release();
  ///

  emit reloadFinished(*this);
}

void IcsService::enableSharing(const std::string &pub, const std::string &pri) {
  this->disableAll();
  for (auto &i : this->connections) {
    if (i.guid() == pub) {
      i.enableAsPublic();
    } else if (i.guid() == pri) {
      i.enableAsPrivate();
    }
  }
}

void IcsService::disableAll() {
  for (auto i : this->connections) {
    if (i.sharingEnabled()) i.config()->DisableSharing();
  }
}

bool IcsService::findById(const std::string &id, IcsConnection &result) {
  for (auto &i : this->connections) {
    if (i.guid() == id) {
      result = i;
      return true;
    }
  }
  return false;
}

IcsService::~IcsService() {
  this->sharing_manager_->Release();
  this->sharing_manager_ = nullptr;
}
}