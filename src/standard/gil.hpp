// Copyright (c) 2023, 2024 Cryolite
// SPDX-License-Identifier: MIT
// This file is part of https://github.com/Cryolite/tsumonya

#define PY_SSIZE_T_CLEAN
#include <Python.h>


namespace Tsumonya::GIL{

class RecursiveLock
{
public:
  RecursiveLock()
    : state_(PyGILState_Ensure())
    , owns_(true)
  {}

  RecursiveLock(RecursiveLock const &) = delete;

  RecursiveLock(RecursiveLock &&rhs) noexcept
    : state_(rhs.state_)
    , owns_(rhs.owns_)
  {
    rhs.owns_ = false;
  }

  ~RecursiveLock()
  {
    if (owns_) {
      PyGILState_Release(state_);
    }
  }

  RecursiveLock &operator=(RecursiveLock const &) = delete;

  RecursiveLock &operator=(RecursiveLock &&rhs) noexcept
  {
    state_ = rhs.state_;
    owns_ = rhs.owns_;
    rhs.owns_ = false;
    return *this;
  }

private:
  PyGILState_STATE state_;
  bool owns_;
}; // class RecursiveLock

} // namespace Tsumonya::GIL
