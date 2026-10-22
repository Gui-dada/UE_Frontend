## ❌ 真正的问题：`EMediaEvent` **不能用于 `DECLARE_DYNAMIC_\*`**

你现在用的是：

```c++
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnCommonMediaEvent,
    EMediaEvent,
    MediaEvent
);
```

⚠️ **致命点在 `DYNAMIC` 上**

------

## UE 的硬规则（必须记住）

> **所有 `DECLARE_DYNAMIC_\*` 委托的参数类型，必须是 UHT 可反射类型**

也就是只能是这些之一：

- `UCLASS`
- `USTRUCT`
- `UENUM(BlueprintType)`
- 基础反射类型（int32 / float / bool / FString / UObject*）

------

## 而 `EMediaEvent` 是什么？

👉 它定义在 `MediaPlayer.h` 里，但它是：

```c++
enum class EMediaEvent : uint8
```

❌ **不是 `UENUM`**
 ❌ **不参与反射**
 ❌ **Blueprint 不认识**

所以：

- C++ 编译器：✅ 能识别
- **UHT（生成 .generated.h）：❌ 直接报错**

这也是你看到那种：

> Unable to find 'class', 'delegate', 'enum', or 'struct'

的**真正原因**

------

## ✅ 正确解决方案（推荐做法）

### ✅ 方案一（最稳）：**自己包一层 Blueprint Enum**

这是 UE 项目里最常见、也是最干净的做法。

### 1️⃣ 定义你自己的枚举

```c++
UENUM(BlueprintType)
enum class ECommonMediaEvent : uint8
{
    None            UMETA(DisplayName = "None"),
    MediaOpened     UMETA(DisplayName = "Media Opened"),
    MediaOpenFailed UMETA(DisplayName = "Media Open Failed"),
    PlaybackEnd     UMETA(DisplayName = "Playback End")
};
```

------

### 2️⃣ Delegate 用你自己的 enum

```c++
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnCommonMediaEvent,
    ECommonMediaEvent,
    MediaEvent
);
```

------

### 3️⃣ 在 C++ 里做一次映射

```c++
void UCommonMediaWidget::HandleMediaEvent(EMediaEvent InEvent)
{
    switch (InEvent)
    {
    case EMediaEvent::MediaOpened:
        OnMediaEvent.Broadcast(ECommonMediaEvent::MediaOpened);
        break;

    case EMediaEvent::MediaOpenFailed:
        OnMediaEvent.Broadcast(ECommonMediaEvent::MediaOpenFailed);
        break;

    case EMediaEvent::PlaybackEndReached:
        OnMediaEvent.Broadcast(ECommonMediaEvent::PlaybackEnd);
        break;

    default:
        break;
    }
}
```

✔ Blueprint
 ✔ C++
 ✔ 架构清晰
 ✔ 不和引擎 enum 强耦合

------

## ❌ 不推荐的方案（但你可能会看到）

### 🚫 改成非 Dynamic Delegate

```c++
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCommonMediaEvent, EMediaEvent);
```

问题：

- ❌ Blueprint 不能绑定
- ❌ UPROPERTY 不能用
- ❌ 不适合 UI / 登录流程

👉 **不适合你的场景**

------

## 🧠 为什么 UE 要这么限制？

因为：

- `DECLARE_DYNAMIC_*` 是给 **反射 + 蓝图 + GC + 序列化** 用的
- 非 UENUM 的 enum：
  - 无法序列化
  - 无法在蓝图里显示
  - UHT 不知道它的存在

这是 **引擎层面的设计限制，不是你写错**

------

## ✅ 总结一句话（记住这个）

> ❗ **凡是 `DECLARE_DYNAMIC_\*`，参数类型必须是 `UENUM / USTRUCT / UObject\*`**