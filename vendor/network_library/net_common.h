#pragma once

#include <memory>
#include <thread>
#include <mutex>
#include <deque>
#include <optional>
#include <vector>
#include <fstream>
#include <iostream>
#include <system_error>
#include <filesystem>
#include <unordered_set>  
#include <algorithm>
#include <chrono>
#include <cstdint>

#define ASIO_STANDALONE
#include "asio.hpp"
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>