#pragma once
#include <filesystem>

namespace fs = std::filesystem;

void delete_slash(fs::path& path);