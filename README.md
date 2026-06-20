# 📄 Protocol

![Protocol Version](https://img.shields.io/badge/Bedrock-v1001%20%7C%2026.30-30ffee?style=flat-square)   

![C++23](https://img.shields.io/badge/C%2B%2B-23-blue)
![CMake](https://img.shields.io/badge/CMake-3.20%2B-064f8c)
![License](https://img.shields.io/badge/License-MPL--2.0-brightgreen)

[📄 中文文档](README.zh.md)

A C++23 Minecraft Bedrock network protocol library.

Protocol provides Bedrock packet models, packet ID mapping, and binary
read/write utilities for building high-performance servers, proxies,
analyzers, and protocol test suites.

## 💬 Community

Join our community:
- Discord: `https://discord.gg/gUQ3Pnssrx`
- QQ Group: `1063182638`

## ✨ Highlights

- C++23-first implementation
- Bedrock packet abstraction via `IPacket` and packet factories
- Binary serialization/deserialization primitives:
  - `BinaryStream`
  - `ReadOnlyBinaryStream`
- CMake package export support (`Protocol::Protocol`)
- MPL-2.0 licensed

## 🧩 Requirements

- CMake 3.20 or newer
- A C++23-capable compiler

## 🛠️ Build

### 📄 Configure

```bash
cmake -S . -B build -G Ninja
```

### 📄 Compile

```bash
cmake --build build
```

### 📄 Install

```bash
cmake --install build
```

## 📚 Used Libraries

The table below lists the main third-party libraries used by the project.
Some entries are optional and only enabled by the corresponding CMake features.

| Library | License | URL |
| --- | --- | --- |
| RakNet | BSD License | https://github.com/SculkCatalystMC/RakNet |
| moodycamel::ConcurrentQueue | Simplified BSD / Boost Software License 1.0 | https://github.com/cameron314/concurrentqueue |
| parallel-hashmap (phmap) | Apache-2.0 | https://github.com/greg7mdp/parallel-hashmap |
| zlib | zlib | https://github.com/madler/zlib |
| Snappy | BSD 3-Clause | https://github.com/google/snappy |
| OpenSSL | Apache-2.0 | https://github.com/openssl/openssl |
| cpp-httplib | MIT | https://github.com/yhirose/cpp-httplib |
| Reflection | MPL-2.0 | https://github.com/SculkCatalystMC/Reflection |
| magic_enum | MIT | https://github.com/Neargye/magic_enum |

## 🔌 CMake Integration

After installation, consume the library with CMake:

```cmake
find_package(Protocol CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Protocol::Protocol)
```

## 🚀 Quick Start

```cpp
#include <sculk/protocol/MinecraftPackets.hpp>
#include <sculk/protocol/packet/IPacket.hpp>
#include <sculk/protocol/utility/BinaryStream.hpp>
#include <sculk/protocol/utility/ReadOnlyBinaryStream.hpp>

#include <cstddef>
#include <vector>

int main() {
    using namespace sculk::protocol;

    // Example input from network transport.
    std::vector<std::byte> input{};

    ReadOnlyBinaryStream readStream{input};
    const auto header = MinecraftPackets::readPacketHeader(readStream);
    auto packet = MinecraftPackets::createPacket(header);

    if (packet) {
        const auto readResult = packet->read(readStream);
        if (!readResult) {
            return 1;
        }

        std::vector<std::byte> output;
        BinaryStream writeStream{output};
        packet->writeWithHeader(writeStream);
    }

    return 0;
}
```

## 🗂️ Project Layout

- `include/sculk/protocol`: public headers
- `src/sculk/protocol`: implementation
- `cmake`: package config templates
- `scripts`: development helper scripts

## 🧬 ABI Namespace

Public APIs are versioned with an inline ABI namespace,
for example `sculk::protocol::abi_v1001`.

## 🤝 Contributing

Contributions are welcome.

- Open an Issue for bug reports, feature requests, or protocol discussions.
- Open a Pull Request for fixes, improvements, and new packet support.
- Keep changes focused and provide clear descriptions of behavior changes.

## 🙏 Acknowledgments

This open source project was born with support from the closed-source paid project [GlacieTeam/ProtocolLib](https://github.com/GlacieTeam/ProtocolLib).

Support link: https://afdian.tv/a/GlacieTeam

## ⚖️ License

This project is licensed under the Mozilla Public License 2.0 (MPL-2.0).
See the full text in [LICENSE](LICENSE).

Core MPL-2.0 requirements (summary):

- If you modify MPL-covered files and distribute them, those modified files must remain under MPL-2.0.
- You must keep existing copyright and license notices in covered source files.
- If you distribute binaries/executables, you must make the corresponding covered source code available.
- You may combine this project with code under other licenses in a larger work, as long as MPL obligations for covered files are respected.

This summary is for convenience only. The [LICENSE](LICENSE) file is the authoritative legal text.

### Copyright © 2026 SculkCatalystMC. All rights reserved.
