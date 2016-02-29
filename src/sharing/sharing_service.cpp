#include "sharing/sharing_service.h"

#include <windows.h>
#include <LM.h>

#include "sharing/shared_resource.h"

namespace sharing {

using sharing::SharedResource;

std::string SharingService::getErrorString(DWORD code) {
  switch (code) {
    case ERROR_ACCESS_DENIED:
      return "ERROR_ACCESS_DENIED";
    case ERROR_INVALID_LEVEL:
      return "ERROR_INVALID_LEVEL";
    case ERROR_INVALID_NAME:
      return "ERROR_INVALID_NAME";
    case NERR_DuplicateShare:
      return "NERR_DuplicateShare";
    case NERR_RedirectedPath:
      return "NERR_RedirectedPath";
    case ERROR_INVALID_PARAMETER:
      return "ERROR_INVALID_PARAMETER";
    case NERR_UnknownDevDir:
      return "UnknownDevDir";
    default:
      return QString("error code %1").arg(code).toStdString();
  }
}

NET_API_STATUS SharingService::add(const SharedResource &res) {
  SHARE_INFO_2 info     = {};
  info.shi2_netname     = const_cast<LPWSTR>(res.name.c_str());
  info.shi2_type        = res.type;
  info.shi2_permissions = res.permissions;
  info.shi2_path        = const_cast<LPWSTR>(res.path.c_str());
  info.shi2_max_uses    = res.max_uses;
  auto ret = NetShareAdd(nullptr, 2, reinterpret_cast<LPBYTE>(&info), nullptr);
  if (ret == NERR_Success) emit this->sharesChanged();

  return ret;
}
NET_API_STATUS SharingService::remove(const std::wstring &name) {
  auto ret = NetShareDel(nullptr, const_cast<LPWSTR>(name.c_str()), 0);
  if (ret == NERR_Success) emit this->sharesChanged();

  return ret;
}
std::vector<SharedResource> SharingService::all() {
  std::vector<SharedResource> entries;

  PSHARE_INFO_2 info, cur;
  NET_API_STATUS status;
  DWORD read = 0, total = 0, resume = 0;
  do {
    status = NetShareEnum(nullptr, 2, reinterpret_cast<LPBYTE *>(&info),
                          MAX_PREFERRED_LENGTH, &read, &total, &resume);
    if (status == ERROR_SUCCESS || status == ERROR_MORE_DATA) {
      cur = info;
      for (auto i = 1; i <= read; i++) {
        SharedResource item;
        item.name        = cur->shi2_netname;
        item.path        = cur->shi2_path;
        item.permissions = cur->shi2_permissions;
        item.type        = cur->shi2_type;
        item.max_uses = cur->shi2_max_uses;
        entries.push_back(item);

        ++cur;
      }
      NetApiBufferFree(info);
    }
  } while (status == ERROR_MORE_DATA);

  return entries;
}
}
