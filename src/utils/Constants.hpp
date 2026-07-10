#pragma once

#include <ccTypes.h>

namespace tinker::constants {

    constexpr float ToolbarHeight = 92.f;

    namespace color_channels {
        constexpr int Background = 1000;
        constexpr int Ground1 = 1001;
        constexpr int Line = 1002;
        constexpr int Line3D = 1003;
        constexpr int Obj = 1004;
        constexpr int PlayerColor1 = 1005;
        constexpr int PlayerColor2 = 1006;
        constexpr int LightBackground = 1007;
        constexpr int Ground2 = 1009;
        constexpr int Black = 1010;
        constexpr int White = 1011;
        constexpr int Lighter = 1012;
        constexpr int Middleground1 = 1013;
        constexpr int Middleground2 = 1014;
    }

    namespace colors {
        constexpr cocos2d::ccColor3B DefaultGround = {166, 166, 166};
        constexpr cocos2d::ccColor3B DefaultMiddleground1 = {100, 100, 100};
        constexpr cocos2d::ccColor3B DefaultMiddleground2 = {150, 150, 150};
    }

    namespace objects {
        constexpr int PlayerTouchToggle = 3643;
        constexpr int ToggleOrb = 1594;
        constexpr int Counter = 1615;
        constexpr int GreenDashOrb = 1704;
        constexpr int PinkDashOrb = 1751;
        constexpr int Checkpoint = 2063;
        constexpr int Particle = 2065;
        constexpr int UserCoin = 1329;
        constexpr int GoldCoin = 142;
        constexpr int OrangeTeleportPortal = 2064;
        constexpr int LinkedTeleportPortal = 747;
        constexpr int LinkedOrangeTeleportPortal = 749;
        constexpr int LargeBeast = 918;
        constexpr int SmallMonster = 1327;
        constexpr int MediumMonster = 1328;
        constexpr int BatMonster = 1584;
        constexpr int SpikeBallMonster = 2012;
        constexpr int AnimatedBlackPit = 919;
        constexpr int Text = 914;
        constexpr int BackgroundColorTrigger = 29;
        constexpr int Ground1ColorTrigger = 30;
        constexpr int ObjColorTrigger = 105;
        constexpr int Line3DColorTrigger = 744;
        constexpr int ColorTrigger = 900;
        constexpr int Ground2ColorTrigger = 900;
        constexpr int LineColorTrigger = 915;
        constexpr int ToggleTrigger = 1049;

        constexpr int NoEnterEffect = 22;

        constexpr int FadeBottomEnterEffect = 23;
        constexpr int FadeTopEnterEffect = 24;
        constexpr int FadeLeftEnterEffect = 25;
        constexpr int FadeRightEnterEffect = 26;

        constexpr int SmallToBigEnterEffect = 27;
        constexpr int BigToSmallEnterEffect = 28;

        constexpr int StartPosition = 31;

        constexpr int EnableGhostTrail = 32;
        constexpr int DisableGhostTrail = 33;

        constexpr int ChaoticEnterEffect = 55;

        constexpr int HalveLeftEnterEffect = 56;
        constexpr int HalveRightEnterEffect = 57;
        constexpr int HalveEnterEffect = 58;
        constexpr int HalveInverseEnterEffect = 59;

        constexpr int NoEnterExitEffect = 1915;

        constexpr int ScaleTrigger = 2067;
        constexpr int GradientTrigger = 2903;
        constexpr int ShaderTrigger = 2904;
        constexpr int ShockWaveShaderTrigger = 2905;
        constexpr int ShockLineShaderTrigger = 2907;
        constexpr int GlitchShaderTrigger = 2909;
        constexpr int ChromaticShaderTrigger = 2910;
        constexpr int ChromaticGlitchShaderTrigger = 2911;
        constexpr int PixelateShaderTrigger = 2912;
        constexpr int LensCircleShaderTrigger = 2913;
        constexpr int RadialBlurShaderTrigger = 2914;
        constexpr int MotionBlurShaderTrigger = 2915;
        constexpr int BulgeShaderTrigger = 2916;
        constexpr int PinchShaderTrigger = 2917;
        constexpr int GrayscaleShaderTrigger = 2919;
        constexpr int SepiaShaderTrigger = 2920;
        constexpr int InvertColorShaderTrigger = 2921;
        constexpr int HueShaderTrigger = 2922;
        constexpr int EditColorShaderTrigger = 2923;
        constexpr int SplitScreenShaderTrigger = 2924;

        constexpr int AreaMoveTrigger = 3006;
        constexpr int AreaRotateTrigger = 3007;
        constexpr int AreaScaleTrigger = 3008;
        constexpr int AreaFadeTrigger = 3009;
        constexpr int AreaTintTrigger = 3010;
        constexpr int AdvancedFollowTrigger = 3016;
        constexpr int EnterMoveTrigger = 3017;
        constexpr int EnterRotateTrigger = 3018;
        constexpr int EnterScaleTrigger = 3019;
        constexpr int EnterFadeTrigger = 3020;
        constexpr int EnterTintTrigger = 3021;
        constexpr int TeleportTrigger = 3022;
        constexpr int EnterStopTrigger = 3023;
        constexpr int AreaStopTrigger = 3024;
        constexpr int EditAdvancedFollowTrigger = 3660;

        constexpr int PulseTrigger = 1006;
        constexpr int AlphaTrigger = 1007;
        constexpr int SpawnTrigger = 1268;
        constexpr int AdvancedRandomTrigger = 2068;

        constexpr int DragModifier = 1755;
        constexpr int JumpModifier = 1813;
        constexpr int StopModifier = 1829;
        constexpr int HeadModifier = 1859;
        constexpr int FlipModifier = 2866;

        constexpr int SFXTrigger = 3602;

        constexpr int TeleportOrb = 3027;

        constexpr int ForceBlock = 2069;
        constexpr int ForceCircle = 3645;
        constexpr int KeyframePoint = 3032;
        constexpr int CameraGuide = 2016;
        constexpr int CollisionBlock = 1816;
        constexpr int BPMTrigger = 3642;

        constexpr int SpikedSquareHazard = 1701;
        constexpr int SpikedCircleHazard = 1702;
        constexpr int TriangleHazard = 1703;

    }
}