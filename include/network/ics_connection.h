#pragma once

#include <string>

#include <NetCon.h>

namespace network
{
class IcsConnection
{
    friend void swap(IcsConnection &lhs, IcsConnection &rhs);

  public:
    IcsConnection() {}
    IcsConnection(INetConnection *conn, INetSharingManager *mgr);
    IcsConnection(const IcsConnection &other);
    IcsConnection &operator=(IcsConnection rhs);
    ~IcsConnection();

    INetConnection *iNetConnection() const {
        return this->conn_;
    }
    INetSharingConfiguration *config() const {
        return this->config_;
    }
    INetConnectionProps *props() const {
        return this->props_;
    }

    std::wstring name() const {
        return this->name_;
    }
    std::string guid() const {
        return this->guid_;
    }
    bool sharingEnabled() const;
    bool isPublic() const;
    bool isPrivate() const;

    void enableAsPrivate();
    void enableAsPublic();
  private:
    void addRef();
    void releaseRef();
    bool valid();

    std::wstring name_;
    std::string guid_;

    INetConnection *conn_             = nullptr;
    INetSharingManager *sharing_mgr_  = nullptr;
    INetSharingConfiguration *config_ = nullptr;
    INetConnectionProps *props_       = nullptr;
};
}