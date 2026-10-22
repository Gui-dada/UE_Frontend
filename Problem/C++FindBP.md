## C++获取蓝图组件示例

要从C++获取`UserWidget`中独有的`MediaPlayer`和`FileMediaPlayer`，并且这些对象没有通过C++暴露给蓝图，首先需要确保你可以访问到`UserWidget`中存储的这些对象。因为这些对象是在`UserWidget`中创建和存储的，你可以通过C++代码在需要的时候访问这些组件或变量。

#### 步骤：

1. ##### **获取`UserWidget`引用**：

    假设你已经在C++中有一个`UserWidget`的引用，你可以直接操作这个`UserWidget`，以便获取它的变量或组件。

2. **访问`MediaPlayer`和`FileMediaPlayer`**：
    为了访问`UserWidget`中的`MediaPlayer`和`FileMediaPlayer`，你需要通过`UserWidget`的函数来查找这些变量或组件。

### 示例代码：

假设你在`UserWidget`类（例如`UG_CommonUserWidget`）中有以下成员：

```c++
// 假设在UG_CommonUserWidget中定义了这些组件
UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
UMediaPlayer* MediaPlayer;

UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
UFileMediaPlayer* FileMediaPlayer;
```

如果这些组件不是通过C++暴露给蓝图（即`BlueprintReadWrite`没有显式地在C++中声明），那么你需要通过`GetWidgetFromName`或者其他类似的方式来动态访问它们。

### 1. 获取`UserWidget`的`MediaPlayer`和`FileMediaPlayer`：

假设你已经获取了`UG_CommonUserWidget`的指针（例如，`MyWidget`），可以通过以下方式访问它的`MediaPlayer`和`FileMediaPlayer`：

```c++
// 获取UserWidget的实例
UG_CommonUserWidget* MyWidget = Cast<UG_CommonUserWidget>(WidgetReference);  // 假设WidgetReference是你的widget引用

if (MyWidget)
{
    // 通过蓝图中的设置直接访问MediaPlayer
    UMediaPlayer* Player = MyWidget->MediaPlayer;
    
    // 通过蓝图中的设置直接访问FileMediaPlayer
    UFileMediaPlayer* FilePlayer = MyWidget->FileMediaPlayer;

    // 使用这些对象做相应的操作
    if (Player)
    {
        // 你可以调用MediaPlayer的方法
    }
    
    if (FilePlayer)
    {
        // 你可以调用FileMediaPlayer的方法
    }
}
```

### 2. 如果变量没有暴露给C++：

如果这些`MediaPlayer`和`FileMediaPlayer`对象没有通过C++暴露给蓝图（即没有`UPROPERTY`声明），你可以通过`GetWidgetFromName`动态获取它们。例如：

```c++
// 获取UserWidget中的子控件
UWidget* MediaPlayerWidget = MyWidget->GetWidgetFromName(TEXT("MediaPlayer"));
UWidget* FileMediaPlayerWidget = MyWidget->GetWidgetFromName(TEXT("FileMediaPlayer"));

if (MediaPlayerWidget)
{
    // 转换为UMediaPlayer类型，确保你知道类型正确
    UMediaPlayer* Player = Cast<UMediaPlayer>(MediaPlayerWidget);
    if (Player)
    {
        // 使用Player进行操作
    }
}

if (FileMediaPlayerWidget)
{
    // 转换为UFileMediaPlayer类型，确保你知道类型正确
    UFileMediaPlayer* FilePlayer = Cast<UFileMediaPlayer>(FileMediaPlayerWidget);
    if (FilePlayer)
    {
        // 使用FilePlayer进行操作
    }
}
```

### 3. 通过`FindWidget`访问（如果是子组件）：

如果这些`MediaPlayer`和`FileMediaPlayer`是作为子组件存在于`UserWidget`中，你可以通过以下方式查找它们：

```c++
UMediaPlayer* MediaPlayer = MyWidget->FindComponentByClass<UMediaPlayer>();
UFileMediaPlayer* FilePlayer = MyWidget->FindComponentByClass<UFileMediaPlayer>();

if (MediaPlayer)
{
    // 使用MediaPlayer
}

if (FilePlayer)
{
    // 使用FilePlayer
}
```

### 总结：

1. 如果这些组件已经暴露为`UPROPERTY`并且绑定了`UserWidget`，你可以直接通过`MyWidget->MediaPlayer`等方式访问它们。
2. 如果没有暴露给C++，你可以使用`GetWidgetFromName`或`FindComponentByClass`来动态查找这些控件或组件。

如果你的`MediaPlayer`和`FileMediaPlayer`是在蓝图中手动创建的且没有暴露给C++，你可能需要使用`FindWidget`或者`GetWidgetFromName`来通过控件名称动态获取它们。