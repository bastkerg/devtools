/*
 * Copyright (c) 2020-2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "CrossPlatformUtils.h"
#include "constants.h"

#include <sys/stat.h>
#include <limits.h>
#include <memory>
#include <unistd.h>

// linux-specific methods

bool CrossPlatformUtils::SetEnv(const std::string& name, const std::string& value)
{
  if (name.empty()) {
    return false;
  }
  return setenv(name.c_str(), value.c_str(), 1) == 0;
}

std::string CrossPlatformUtils::GetExecutablePath(std::error_code& ec) {
  struct stat sb;
  ssize_t bytesRead, bufSize;

  // This might fail in case of "/proc/self/exe" not available
  if (lstat(PROC_SELF_EXE, &sb) == -1) {
    ec.assign(errno, std::generic_category());
    return "";
  }
  bufSize = sb.st_size;

  if (0 == sb.st_size) {
    bufSize = PATH_MAX;
  }

  std::unique_ptr<char[]> exePath{};
  try {
    exePath = std::make_unique<char[]>(bufSize);
  }
  catch(std::bad_alloc&) {
    ec.assign(ENOMEM, std::generic_category());
    return "";
  }

  bytesRead = readlink(PROC_SELF_EXE, exePath.get(), bufSize);
  if (bytesRead < 0) {
    ec.assign(errno, std::generic_category());
    return "";
  }

  if (bytesRead >= bufSize) {
    ec.assign(EOVERFLOW, std::generic_category());
    bytesRead = bufSize;
  }

  return std::string(exePath.get(), bytesRead);
}

std::string CrossPlatformUtils::GetRegistryString(const std::string& key)
{
  return GetEnv(key); // non-Windows implementation returns environment variable value
}

bool CrossPlatformUtils::CanExecute(const std::string& file)
{
  return access(file.c_str(), X_OK) == 0;
}

CrossPlatformUtils::REG_STATUS CrossPlatformUtils::GetLongPathRegStatus() {
  return REG_STATUS::NOT_SUPPORTED;
}

std::filesystem::perms CrossPlatformUtils::GetCurrentUmask() {
  // Only way to get current umask is to change it, so revert the possible change directly.
  mode_t value = ::umask(0);
  ::umask(value);

  std::filesystem::perms perm = std::filesystem::perms::none;

  // Map the mode_t value to coresponding fs::perms value
  constexpr struct {
    mode_t m;
    std::filesystem::perms p;
  } mode_to_perm_map[] = {
    {S_IRUSR, std::filesystem::perms::owner_read},
    {S_IWUSR, std::filesystem::perms::owner_write},
    {S_IXUSR, std::filesystem::perms::owner_exec},
    {S_IRGRP, std::filesystem::perms::group_read},
    {S_IWGRP, std::filesystem::perms::group_write},
    {S_IXGRP, std::filesystem::perms::group_exec},
    {S_IROTH, std::filesystem::perms::others_read},
    {S_IWOTH, std::filesystem::perms::others_write},
    {S_IXOTH, std::filesystem::perms::others_exec}
  };
  for (auto [m, p] : mode_to_perm_map) {
    if (value & m) {
      perm |= p;
    }
  }

  return perm;
}
// end of Utils.cpp
