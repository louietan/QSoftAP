#pragma once

#include <vector>

#include <QObject>

#include <windows.h>
#include <Lmshare.h>

#include "commons.h"

class QString;

namespace sharing
{
struct SharedResource;

/**
 * 提供管理资源共享功能。对native api的简单封装
 */
class SharingService : public QObject
{
    Q_OBJECT

    SINGLETON(SharingService)
  signals:
    void sharesChanged();
  public:
    static QString getErrorString(DWORD code);

    NET_API_STATUS add(const SharedResource &res);
    NET_API_STATUS remove(const std::wstring &name);
    std::vector<SharedResource> all();
  protected:
    SharingService() {}
};
}