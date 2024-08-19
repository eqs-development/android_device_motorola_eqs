#
# Copyright (C) 2022 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

DEVICE_PATH := device/motorola/eqs

# Inherit from motorola sm8475-common
include device/motorola/sm8475-common/BoardConfigCommon.mk

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := eqs

# HIDL
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += $(DEVICE_PATH)/device_framework_matrix_eqs.xml
ODM_MANIFEST_SKUS += dne
ODM_MANIFEST_DNE_FILES := $(DEVICE_PATH)/manifest_eqs_dne.xml
ODM_MANIFEST_SKUS += dn
ODM_MANIFEST_DN_FILES := $(DEVICE_PATH)/manifest_eqs_dn.xml

# Kernel
TARGET_KERNEL_CONFIG += \
	vendor/ext_config/moto-waipio-eqs.config

# Partitions
ifneq ($(WITH_GMS),true)
BOARD_PRODUCTIMAGE_PARTITION_RESERVED_SIZE := 3318226944
BOARD_SYSTEMIMAGE_PARTITION_RESERVED_SIZE := 873680896
BOARD_SYSTEM_EXTIMAGE_PARTITION_RESERVED_SIZE := 916299776
endif
BOARD_MOT_DP_GROUP_SIZE := 11806965760 # ( BOARD_SUPER_PARTITION_SIZE - 4MB )
BOARD_SUPER_PARTITION_SIZE := 11811160064

# Properties
TARGET_PRODUCT_PROP += $(DEVICE_PATH)/product.prop
TARGET_SYSTEM_PROP += $(DEVICE_PATH)/system.prop
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# Recovery
TARGET_RECOVERY_UI_MARGIN_HEIGHT := 90

# Security
BOOT_SECURITY_PATCH := 2024-06-01
VENDOR_SECURITY_PATCH := $(BOOT_SECURITY_PATCH)

# Verified Boot
BOARD_AVB_ROLLBACK_INDEX := 18
BOARD_AVB_VBMETA_SYSTEM_ROLLBACK_INDEX := 18

# inherit from the proprietary version
include vendor/motorola/eqs/BoardConfigVendor.mk
