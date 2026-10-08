#pragma once
#include <optional>
#include <string>

enum class BackendKind { Sequential, OpenMP };

inline std::optional<BackendKind> parseBackendKind(std::string s) {
  if (s == "seq")
    return BackendKind::Sequential;
  if (s == "openmp")
    return BackendKind::OpenMP;
  return std::nullopt;
}
