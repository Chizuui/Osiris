#pragma once

#include <array>
#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include "../SDK/ItemSchema.h"
#include "StaticData.h"

#include "GameItems/Item.h"

enum class Team;

enum TournamentTeam : std::uint8_t;
enum TournamentStage : std::uint8_t;
enum ProPlayer;

namespace inventory
{

struct StickerConfig {
    int stickerID = 0;
    float wear = 0.0f;
};

struct Skin {
    float wear = 0.0f;
    int seed = 1;
    int statTrak = -1;
    std::uint32_t tournamentID = 0;
    std::array<StickerConfig, 5> stickers;
    std::string nameTag;
    TournamentStage tournamentStage{};
    TournamentTeam tournamentTeam1{};
    TournamentTeam tournamentTeam2{};
    ProPlayer proPlayer{};

    [[nodiscard]] bool isSouvenir() const noexcept { return tournamentID != 0; }
};

struct PatchConfig {
    int patchID = 0;
};

struct DynamicAgentData {
    std::array<PatchConfig, 5> patches;
};

struct DynamicGloveData {
    float wear = 0.0f;
    int seed = 1;
};

struct DynamicMusicData {
    int statTrak = -1;
};

struct DynamicSouvenirPackageData {
    TournamentStage tournamentStage{};
    TournamentTeam tournamentTeam1{};
    TournamentTeam tournamentTeam2{};
    ProPlayer proPlayer{};
};

struct DynamicServiceMedalData {
    std::uint32_t issueDateTimestamp = 0;
};

struct DynamicTournamentCoinData {
    std::uint32_t dropsAwarded = 0;
};

struct DynamicGraffitiData {
    std::int8_t usesLeft = -1;
};

struct Item {
private:
    std::optional<std::reference_wrapper<const game_items::Item>> item;
    std::size_t dynamicDataIndex = static_cast<std::size_t>(-1);
    bool toDelete = false;
public:
    explicit Item(const game_items::Item& item, std::size_t dynamicDataIndex) noexcept : item{ item }, dynamicDataIndex{ dynamicDataIndex } {}

    void markAsDeleted() noexcept { item.reset(); }
    bool isDeleted() const noexcept { return !item.has_value(); }
    void markToDelete() noexcept { toDelete = true; }
    bool shouldDelete() const noexcept { return toDelete; }
    bool isValid() const noexcept { return !isDeleted() && !shouldDelete(); }

    bool isSticker() const noexcept { return isValid() && get().isSticker(); }
    bool isSkin() const noexcept { return isValid() && get().isSkin(); }
    bool isGlove() const noexcept { return isValid() && get().isGloves(); }
    bool isMusic() const noexcept { return isValid() && get().isMusic(); }
    bool isAgent() const noexcept { return isValid() && get().isAgent(); }
    bool isCollectible() const noexcept { return isValid() && get().isCollectible(); }
    bool isCase() const noexcept { return isValid() && get().isCase(); }
    bool isCaseKey() const noexcept { return isValid() && get().isCaseKey(); }
    bool isGraffiti() const noexcept { return isValid() && get().isGraffiti(); }
    bool isOperationPass() const noexcept { return isValid() && get().isOperationPass(); }
    bool isNameTag() const noexcept { return isValid() && get().isNameTag(); }
    bool isPatch() const noexcept { return isValid() && get().isPatch(); }
    bool isStatTrakSwapTool() const noexcept { return isValid() && get().isStatTrakSwapTool(); }
    bool isViewerPass() const noexcept { return isValid() && get().isViewerPass(); }
    bool isServiceMedal() const noexcept { return isValid() && get().isServiceMedal(); }
    bool isSouvenirToken() const noexcept { return isValid() && get().isSouvenirToken(); }
    bool isTournamentCoin() const noexcept { return isValid() && get().isTournamentCoin(); }

    std::size_t getDynamicDataIndex() const noexcept { assert(dynamicDataIndex != static_cast<std::size_t>(-1)); return dynamicDataIndex; }

    const game_items::Item& get() const noexcept { assert(isValid()); return item.value(); }
};

}

namespace Inventory
{
    constexpr auto InvalidDynamicDataIdx = static_cast<std::size_t>(-1);
    constexpr auto BASE_ITEMID = 1152921504606746975;

    std::vector<inventory::Item>& get() noexcept;
    void addItemUnacknowledged(const game_items::Item& gameItem, std::size_t dynamicDataIdx) noexcept;
    void addItemAcknowledged(const game_items::Item& gameItem, std::size_t dynamicDataIdx) noexcept;
    std::uint64_t addItemNow(const game_items::Item& gameItem, std::size_t dynamicDataIdx, bool asUnacknowledged) noexcept;
    void deleteItemNow(std::uint64_t itemID) noexcept;
    void runFrame() noexcept;
    inventory::Item* getItem(std::uint64_t itemID) noexcept;
    std::uint64_t recreateItem(std::uint64_t itemID) noexcept;
    void clear() noexcept;
    void equipItem(Team team, int slot, std::size_t index) noexcept;
    std::size_t getItemIndex(std::uint64_t itemID) noexcept;

    inventory::Skin& dynamicSkinData(const inventory::Item& item) noexcept;
    inventory::DynamicGloveData& dynamicGloveData(const inventory::Item& item) noexcept;
    inventory::DynamicAgentData& dynamicAgentData(const inventory::Item& item) noexcept;
    inventory::DynamicMusicData& dynamicMusicData(const inventory::Item& item) noexcept;
    inventory::DynamicSouvenirPackageData& dynamicSouvenirPackageData(const inventory::Item& item) noexcept;
    inventory::DynamicServiceMedalData& dynamicServiceMedalData(const inventory::Item& item) noexcept;
    inventory::DynamicTournamentCoinData& dynamicTournamentCoinData(const inventory::Item& item) noexcept;
    inventory::DynamicGraffitiData& dynamicGraffitiData(const inventory::Item& item) noexcept;

    std::size_t emplaceDynamicData(inventory::Skin&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicGloveData&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicAgentData&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicMusicData&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicSouvenirPackageData&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicServiceMedalData&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicTournamentCoinData&& data) noexcept;
    std::size_t emplaceDynamicData(inventory::DynamicGraffitiData&& data) noexcept;
}
