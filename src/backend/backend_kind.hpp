#pragma once
#include <optional>
#include <string>

enum class BackendKind { Sequential };

inline std::optional<BackendKind> parseBackendKind(std::string s) {
  if (s == "seq")
    return BackendKind::Sequential;
  return std::nullopt;
}
