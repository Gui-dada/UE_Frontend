## C++

### Unreal Engine Actor 和 Component 的主要生命周期函数执行顺序如下：

1. 构造函数 (Constructor)

- 调用时间：
  - 编辑器加载时（编辑器中对象实例化，用于设置默认值）
  - 运行时 Spawn/加载时
- 作用：初始化默认变量、创建组件、设置属性的默认状态。
- 特点：不适合进行运行时逻辑或依赖游戏世界状态的初始化。

2. PostInitProperties

- 调用时间：构造函数后自动调用。
- 作用：完成属性初始化，针对反序列化后的对象进行对属性的修正或初始化。

3. OnConstruction (或 Construction Script 蓝图)

- 调用时间：Actor被放置到关卡或属性更改时（编辑器和运行时都可能触发）。
- 作用：根据变量更新组件和外观，为编辑和运行时的视觉反馈做准备。
- 特点：多次调用，依赖于改变，适合构造阶段对组件的动态调整。

4. PreInitializeComponents

- 调用时间：BeginPlay前，组件初始化之前（只在运行时调用）。
- 作用：执行在初始化组件前需要的准备工作。

5. InitializeComponent (组件的初始化)

- 调用时间：组件逐一初始化。
- 作用：组件自己的初始化代码运行，如绑定事件、初始化状态。

6. PostInitializeComponents

- 调用时间：所有组件初始化完成后。
- 作用：确保组件准备就绪，进行需要全部组件初始化后的设置。

7. BeginPlay

- 调用时间：游戏开始或Actor被激活时执行，仅运行时。
- 作用：游戏逻辑真正开始，运行需要游戏状态支持的代码。
- 特点：只调用一次，适合启动时的初始化和逻辑启动。

8. Tick

- 调用时间：BeginPlay后每帧调用（如果启用Tick）。
- 作用：每帧更新逻辑。

#### 简要执行顺序示例

```  text
Constructor -> PostInitProperties -> OnConstruction -> PreInitializeComponents -> InitializeComponent -> PostInitializeComponents -> BeginPlay -> Tick
```





#### 小贴士

- **构造函数**适合做默认变量和组件创建，**避免做运行时代码**。
- **OnConstruction**适合调整组件、视觉效果，编辑器和运行时都会调用。
- **InitializeComponent**及其相关生命周期用于组件层级初始化。
- **BeginPlay**是游戏启动后激活的真正“开始”，运行时逻辑应放这里。
- 不同阶段调用时机不同，编辑器和游戏运行时会有差异，建议初始化场景数据用`BeginPlay`。