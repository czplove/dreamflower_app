#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=dreamflower
PKG_VERSION:=0.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
#PKG_SOURCE_URL:=@SF/serialconsole/sc
#PKG_MD5SUM:=4fe58576a5bddb9f1fb3dcfc6410b885
PKG_MAINTAINER:=taidalab <zdajun1988@163.com>

PKG_BUILD_DIR=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/dreamflower_app
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Serial Console - minimal terminal program
  URL:=http://sourceforge.net/projects/serialconsole/
endef

define Package/dreamflower_app/description
serialconsole (sc) is a minimal terminal program allowing to use one machine
to access the serial console of another machine.
endef

define Package/dreamflower_app/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dreamflower $(1)/usr/bin/
endef

$(eval $(call BuildPackage,dreamflower_app))
