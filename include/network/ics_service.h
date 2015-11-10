#pragma once

#include <vector>

#include <QObject>

#include "commons.h"

struct INetSharingManager;

namespace network {

class IcsConnection;

//提供ICS管理的功能
class IcsService : public QObject {
  Q_OBJECT

  SINGLETON(IcsService)
 public:
  ~IcsService();

  void reloadConnections();

  /**
   * 启用共享
   *
   * @param[in] pub 作为public连接的guid
   * @param[in] prv 作为private连接的guid
   */
  void enableSharing(const std::string &pub, const std::string &prv);
  void disableAll();

  /**
   * 根据id找连接
   *
   * @param[in] id
   * @param[out] result
   * @return 是否找到
   */
  bool findById(const std::string &id, IcsConnection &result);
  std::vector<IcsConnection> connections;

 signals:
  //连接加载完后发出
  void reloadFinished(IcsService &sender);

 protected:
  IcsService();

 private:
  INetSharingManager *sharing_manager_ = nullptr;
};
}