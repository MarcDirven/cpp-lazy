module;

#ifndef __cpp_modules
#    error Modules are not supported for this compiler and or C++ version confugration.
#endif

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <concepts>
#include <execution>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

export module lz;

#define LZ_MODULE_EXPORT export
#define LZ_MODULE_EXPORT_SCOPE_BEGIN export {
#define LZ_MODULE_EXPORT_SCOPE_END }

export {
#include "Lz/CString.hpp"
#include "Lz/CartesianProduct.hpp"
#include "Lz/ChunkIf.hpp"
#include "Lz/Chunks.hpp"
#include "Lz/Concatenate.hpp"
#include "Lz/Enumerate.hpp"
#include "Lz/Except.hpp"
#include "Lz/Exclude.hpp"
#include "Lz/Filter.hpp"
#include "Lz/Flatten.hpp"
#include "Lz/FunctionTools.hpp"
#include "Lz/Generate.hpp"
#include "Lz/GroupBy.hpp"
#include "Lz/Join.hpp"
#include "Lz/JoinWhere.hpp"
#include "Lz/Loop.hpp"
#include "Lz/Lz.hpp"
#include "Lz/Map.hpp"
#include "Lz/Random.hpp"
#include "Lz/Range.hpp"
#include "Lz/Repeat.hpp"
#include "Lz/Rotate.hpp"
#include "Lz/StringSplitter.hpp"
#include "Lz/Take.hpp"
#include "Lz/TakeEvery.hpp"
#include "Lz/Unique.hpp"
#include "Lz/Zip.hpp"
#include "Lz/ZipLongest.hpp"
}