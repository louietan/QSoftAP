#pragma once

#include <string>

#include <windows.h>
#include <Lmshare.h>
#include <LMaccess.h>

namespace sharing {

/**
 * 表示一个共享资源
 */
struct SharedResource {
  std::wstring name;
  std::wstring path;

  /**
   * 资源类型
   *
   * STYPE_DISKTREE:Disk drive.
   * STYPE_PRINTQ:Print queue.
   * STYPE_DEVICE:Communication device.
   * STYPE_IPC:Interprocess communication(IPC).
   */
  DWORD type = STYPE_DISKTREE;

  DWORD permissions = ACCESS_ALL;
  int max_uses      = -1;
};

}