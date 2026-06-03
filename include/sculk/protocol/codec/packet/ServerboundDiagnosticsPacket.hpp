// Copyright © 2026 SculkCatalystMC. All rights reserved.
//
// This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
// distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include "sculk/protocol/codec/packet/IPacket.hpp"
#include <vector>

namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE {

class ServerboundDiagnosticsPacket : public IPacket {
public:
public:
    enum class MemoryCategory : std::uint8_t {
        Unknown                                 = 0,
        Invalid_SizeUnknown                     = 1,
        Actor                                   = 2,
        ActorAnimation                          = 3,
        ActorRendering                          = 4,
        BlockTickingQueues                      = 5,
        Biome_Storage                           = 6,
        Cereal                                  = 7,
        CircuitSystem                           = 8,
        Client                                  = 9,
        Commands                                = 10,
        DBStorage                               = 11,
        Debug                                   = 12,
        Documentation                           = 13,
        ECSSystems                              = 14,
        FMOD                                    = 15,
        Fonts                                   = 16,
        ImGui                                   = 17,
        Input                                   = 18,
        JsonUI                                  = 19,
        JsonUI_ControlFactory_Json              = 20,
        JsonUI_ControlTree                      = 21,
        JsonUI_ControlTree_ControlElement       = 22,
        JsonUI_ControlTree_PopulateDataBinding  = 23,
        JsonUI_ControlTree_PopulateFocus        = 24,
        JsonUI_ControlTree_PopulateLayout       = 25,
        JsonUI_ControlTree_PopulateOther        = 26,
        JsonUI_ControlTree_PopulateSprite       = 27,
        JsonUI_ControlTree_PopulateText         = 28,
        JsonUI_ControlTree_PopulateTTS          = 29,
        JsonUI_ControlTree_Visibility           = 30,
        JsonUI_CreateUI                         = 31,
        JsonUI_Defs                             = 32,
        JsonUI_LayoutManager                    = 33,
        JsonUI_LayoutManager_RemoveDependencies = 34,
        JsonUI_LayoutManager_InitVariable       = 35,
        Languages                               = 36,
        Level                                   = 37,
        LevelStructures                         = 38,
        LevelChunk                              = 39,
        LevelChunkGen                           = 40,
        LevelChunkGenThreadLocal                = 41,
        LightVolumeManager                      = 42,
        Network                                 = 43,
        Marketplace                             = 44,
        Material_DragonCompiledDefinition       = 45,
        Material_DragonMaterial                 = 46,
        Material_DragonResource                 = 47,
        Material_DragonUniformMap               = 48,
        Material_RenderMaterial                 = 49,
        Material_RenderMaterialGroup            = 50,
        Material_VariationManager               = 51,
        Molang                                  = 52,
        OreUI                                   = 53,
        Persona                                 = 54,
        Player                                  = 55,
        RenderChunk                             = 56,
        RenderChunk_IndexBuffer                 = 57,
        RenderChunk_VertexBuffer                = 58,
        Rendering                               = 59,
        Rendering_RenderRegistry                = 60,
        Rendering_Library                       = 61,
        RequestLog                              = 62,
        ResourcePacks                           = 63,
        Sound                                   = 64,
        SubChunk_BiomeData                      = 65,
        SubChunk_BlockData                      = 66,
        SubChunk_LightData                      = 67,
        Textures                                = 68,
        WeatherRenderer                         = 69,
        World_Generator                         = 70,
        Tasks                                   = 71,
        Test                                    = 72,
        Scripting                               = 73,
        Scripting_Runtime                       = 74,
        Scripting_Context                       = 75,
        Scripting_Context_Bindings_MC           = 76,
        Scripting_Context_Bindings_GT           = 77,
        Scripting_Context_Run                   = 78,
        DataDrivenUI                            = 79,
        DataDrivenUI_Defs                       = 80,
        Gameface                                = 81,
        Gameface_System                         = 82,
        Gameface_DOM                            = 83,
        Gameface_CSS                            = 84,
        Gameface_Display                        = 85,
        Gameface_TempAllocator                  = 86,
        Gameface_PoolAllocator                  = 87,
        Gameface_Dump                           = 88,
        Gameface_Media                          = 89,
        Gameface_JSON                           = 90,
        Gameface_ScriptEngine                   = 91,
    };

    struct MemoryCategoryCounter {
        MemoryCategory mType{};
        std::uint64_t  mCurrentBytes{};

        void write(BinaryStream& stream) const;

        [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
    };

    struct EntityDiagnosticTimingInfo {
        std::string   mDisplayName{};
        std::string   mEntity{};
        std::uint64_t mTimeInNanoseconds{};
        std::uint8_t  mPercentOfTotal{};

        void write(BinaryStream& stream) const;

        [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
    };

    struct SystemDiagnosticTimingInfo {
        std::string   mDisplayName{};
        std::uint64_t mSystemIndex{};
        std::uint64_t mTimeInNanoseconds{};
        std::uint8_t  mPercentOfTotal{};

        void write(BinaryStream& stream) const;

        [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream);
    };

public:
    float                                   mAvgFps{};
    float                                   mAvgServerSimTickTimeMS{};
    float                                   mAvgClientSimTickTimeMS{};
    float                                   mAvgBeginFrameTimeMS{};
    float                                   mAvgInputTimeMS{};
    float                                   mAvgRenderTimeMS{};
    float                                   mAvgEndFrameTimeMS{};
    float                                   mAvgRemainderTimePercent{};
    float                                   mAvgUnaccountedTimePercent{};
    std::vector<MemoryCategoryCounter>      mMemoryCategoryValues{};
    std::vector<EntityDiagnosticTimingInfo> mEntityDiagnostics{};
    std::vector<SystemDiagnosticTimingInfo> mSystemDiagnostics{};

public:
    [[nodiscard]] MinecraftPacketIds getId() const noexcept override;

    [[nodiscard]] std::string_view getName() const noexcept override;

    void write(BinaryStream& stream) const override;

    [[nodiscard]] Result<> read(ReadOnlyBinaryStream& stream) override;

    SCULK_PROTOCOL_PACKET_TO_STRING();
};

} // namespace sculk::protocol::SCULK_ABI_INLINE_NAMESPACE

SCULK_PROTOCOL_PACKET_FORMATTER(ServerboundDiagnosticsPacket)