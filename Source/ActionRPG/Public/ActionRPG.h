#pragma once

//-------------------------------------------------
// 我们在这里定义了一个基本的头文件，他包含了一些常用的引擎功能
// 如果这个项目里面其他的头文件需要使用这里面的功能，请直接引用这个头文件
// 在此头文件中，我们引用了 `EngineMinimal` 和 `Engine/Engine.h`，我们常用的功能都会在这两个头文件里面
// 我们没有直接引用 `Engine.h`,因为他有很多我们用不到的功能，而且会降低编译的速度
//-------------------------------------------------

#include "RPGTypes.h"
#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

// 以 C++ extern 的方式声明一个日志名称： LogActionRPG 
ACTIONRPG_API DECLARE_LOG_CATEGORY_EXTERN(LogActionRPG, Log, All);

