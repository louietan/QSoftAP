#include "network/ics_connection.h"

#include <array>

namespace network {

void swap(IcsConnection &lhs, IcsConnection &rhs) {
  std::swap(lhs.name_, rhs.name_);
  std::swap(lhs.conn_, rhs.conn_);
  std::swap(lhs.sharing_mgr_, rhs.sharing_mgr_);
  std::swap(lhs.config_, rhs.config_);
  std::swap(lhs.props_, rhs.props_);
  std::swap(lhs.guid_, rhs.guid_);
}

IcsConnection::IcsConnection(INetConnection *conn, INetSharingManager *mgr)
    : conn_(conn), sharing_mgr_(mgr) {
  this->sharing_mgr_->get_INetSharingConfigurationForINetConnection(
      this->conn_, &this->config_);
  this->sharing_mgr_->get_NetConnectionProps(this->conn_, &this->props_);
  this->addRef();

  BSTR temp;

  this->props_->get_Name(&temp);
  this->name_ = temp;
  SysFreeString(temp);

  this->props_->get_Guid(&temp);
  std::array<char, 100> buffer;
  wcstombs(buffer.data(), temp, buffer.size());
  this->guid_ = buffer.data();
  SysFreeString(temp);
}

IcsConnection::IcsConnection(const IcsConnection &other) {
  this->name_        = other.name_;
  this->guid_        = other.guid_;
  this->conn_        = other.conn_;
  this->config_      = other.config_;
  this->props_       = other.props_;
  this->sharing_mgr_ = other.sharing_mgr_;
  this->addRef();
}

IcsConnection &IcsConnection::operator=(IcsConnection rhs) {
  if (&rhs != this) {
    this->releaseRef();
    swap(*this, rhs);
    this->addRef();
  }
  return *this;
}

IcsConnection::~IcsConnection() { this->releaseRef(); }

bool IcsConnection::sharingEnabled() const {
  VARIANT_BOOL sharing_enabled;
  this->config_->get_SharingEnabled(&sharing_enabled);
  return sharing_enabled;
}

bool IcsConnection::isPublic() const {
  SHARINGCONNECTIONTYPE sharing_type;
  this->config_->get_SharingConnectionType(&sharing_type);
  return sharing_type == SHARINGCONNECTIONTYPE::ICSSHARINGTYPE_PUBLIC;
}

bool IcsConnection::isPrivate() const {
  SHARINGCONNECTIONTYPE sharing_type;
  this->config_->get_SharingConnectionType(&sharing_type);
  return sharing_type == SHARINGCONNECTIONTYPE::ICSSHARINGTYPE_PRIVATE;
}

void IcsConnection::enableAsPrivate() {
  this->config()->EnableSharing(SHARINGCONNECTIONTYPE::ICSSHARINGTYPE_PRIVATE);
}

void IcsConnection::enableAsPublic() {
  this->config()->EnableSharing(SHARINGCONNECTIONTYPE::ICSSHARINGTYPE_PUBLIC);
}

void IcsConnection::addRef() {
  if (this->valid()) {
    this->conn_->AddRef();
    this->config_->AddRef();
    this->props_->AddRef();
  }
}

void IcsConnection::releaseRef() {
  if (this->valid()) {
    this->conn_->Release();
    this->config_->Release();
    this->props_->Release();
  }
}

bool IcsConnection::valid() {
  return this->conn_ && this->config_ && this->props_;
}
}