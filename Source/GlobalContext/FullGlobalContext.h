#pragma once

#include <CS2/Classes/CLoopModeGame.h>
#include <GameClasses/Implementation/GameClassImplementations.h>
#include <GameDLLs/Tier0Dll.h>
#include <FeatureHelpers/FeatureHelpers.h>
#include <FeatureHelpers/Sound/SoundWatcher.h>
#include <Features/Features.h>
#include <Helpers/PatternNotFoundLogger.h>
#include <Helpers/UnloadFlag.h>
#include <Hooks/Hooks.h>
#include <Hooks/PeepEventsHook.h>
#include <MemoryPatterns/ClientModePatterns.h>
#include <MemoryPatterns/ClientPatterns.h>
#include <MemoryPatterns/FileSystemPatterns.h>
#include <MemoryPatterns/GameRulesPatterns.h>
#include <MemoryPatterns/MemAllocPatterns.h>
#include <MemoryPatterns/PanelPatterns.h>
#include <MemoryPatterns/PanelStylePatterns.h>
#include <MemoryPatterns/PanoramaImagePanelPatterns.h>
#include <MemoryPatterns/PanoramaLabelPatterns.h>
#include <MemoryPatterns/PanoramaUiEnginePatterns.h>
#include <MemoryPatterns/PanoramaUiPanelPatterns.h>
#include <MemoryPatterns/PlantedC4Patterns.h>
#include <MemoryPatterns/SoundSystemPatterns.h>
#include <MemorySearch/PatternFinder.h>
#include <UI/Panorama/PanoramaGUI.h>
#include <Platform/DynamicLibrary.h>
#include <Platform/VmtFinder.h>
#include <Vmt/VmtLengthCalculator.h>

#include "PeepEventsHookResult.h"

struct FullGlobalContext {
    FullGlobalContext(PeepEventsHook peepEventsHook, DynamicLibrary clientDLL, DynamicLibrary panoramaDLL, const PatternFinder<PatternNotFoundLogger>& clientPatternFinder, const PatternFinder<PatternNotFoundLogger>& panoramaPatternFinder, const PatternFinder<PatternNotFoundLogger>& soundSystemPatternFinder, const FileSystemPatterns& fileSystemPatterns) noexcept
        : _gameClasses{
            ClientModePatterns{clientPatternFinder},
            ClientPatterns{clientPatternFinder},
            fileSystemPatterns,
            GameRulesPatterns{clientPatternFinder},
            MemAllocPatterns{clientPatternFinder},
            PanelPatterns{clientPatternFinder},
            PanelStylePatterns{panoramaPatternFinder},
            PanoramaImagePanelPatterns{clientPatternFinder},
            PanoramaLabelPatterns{clientPatternFinder},
            PanoramaUiEnginePatterns{panoramaPatternFinder},
            PanoramaUiPanelPatterns{clientPatternFinder, panoramaPatternFinder},
            PlantedC4Patterns{clientPatternFinder},
            Tier0Dll{}}
        , hooks{
            peepEventsHook,
            ClientPatterns{clientPatternFinder},
            VmtLengthCalculator{clientDLL.getCodeSection(), clientDLL.getVmtSection()}}
        , soundWatcher{
            fileSystemPatterns,
            SoundSystemPatterns{soundSystemPatternFinder}}
        , featureHelpers{
            ClientPatterns{clientPatternFinder},
            PanelStylePatterns{panoramaPatternFinder},
            VmtFinder{panoramaDLL.getVmtFinderParams()}}
        , features{
            ClientPatterns{clientPatternFinder},
            featureHelpers.sniperScopeBlurRemover,
            hooks.loopModeGameHook,
            hooks.viewRenderHook,
            soundWatcher}
    {
    }

    [[nodiscard]] const GameClassImplementations& gameClasses() const noexcept
    {
        return _gameClasses;
    }
    
    void onRenderStart(cs2::CViewRender* thisptr) noexcept
    {
        hooks.viewRenderHook.getOriginalOnRenderStart()(thisptr);
        if (featureHelpers.globalVarsProvider && featureHelpers.globalVarsProvider.getGlobalVars())
            soundWatcher.update(featureHelpers.globalVarsProvider.getGlobalVars()->curtime);
        features.soundFeatures.runOnViewMatrixUpdate(featureHelpers.getSoundVisualizationHelpers());
    }

    [[nodiscard]] PeepEventsHookResult onPeepEventsHook() noexcept
    {
        features.hudFeatures.bombTimer.run(featureHelpers.getBombTimerHelpers());
        features.hudFeatures.defusingAlert.run(featureHelpers.getDefusingAlertHelpers());
        features.hudFeatures.killfeedPreserver.run(featureHelpers.getKillfeedPreserverHelpers());

        UnloadFlag unloadFlag;
        panoramaGUI.run(features, unloadFlag);
        hooks.update();
        features.visuals.scopeOverlayRemover.updatePanelVisibility(featureHelpers.hudProvider);

        if (unloadFlag)
            hooks.forceUninstall();

        return PeepEventsHookResult{hooks.peepEventsHook.original, static_cast<bool>(unloadFlag)};
    }

    [[nodiscard]] cs2::CLoopModeGame::getWorldSession getWorldSessionHook(ReturnAddress returnAddress) noexcept
    {
        featureHelpers.sniperScopeBlurRemover.getWorldSessionHook(returnAddress);
        return hooks.loopModeGameHook.originalGetWorldSession;
    }

private:
    GameClassImplementations _gameClasses;
    Hooks hooks;
    SoundWatcher soundWatcher;
    FeatureHelpers featureHelpers;
    Features features;
public:
    PanoramaGUI panoramaGUI;
};