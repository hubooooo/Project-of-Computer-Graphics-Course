基于 QT + CHAI3D 开发的一款图形交互软件



所设计的软件能实现显示模型的边缘、表面法向量、包围盒、网格。

边缘显示实现代码：
// 计算相邻三角形角度超过一定角度的所有边;
// 设置边线和颜色的线宽
cColorf color;
color.setBlack();
mesh->setEdgeProperties(1, color); 
// 启用曲面和边
mesh->setShowTriangles(true);
mesh->setShowEdges(true);
// 仅启用边
mesh->setShowTriangles(false);
mesh->setShowEdges(true);

 
图2.7 边缘显示


表面法向显示实现代码：
//计算所有曲面法线
mesh->computeAllNormals();
//设置显示的常规属性
cColorf color;
color.setOrangeTomato();
mesh->setNormalsProperties(0.01, color);
//显示表面法线
mesh->setShowNormals(true);
 
图2.8 表面法向量显示


模型包围盒显示实现代码：
// 计算边界盒
        m_mesh->computeBoundaryBox(true);
// 计算碰撞检测算法
        m_mesh->createAABBCollisionDetector(m_toolRadius);
// 计算物体所有的边
        m_mesh->computeAllEdges(15.0);
// 设置线宽
        cColorf colorEdges;
        colorEdges.setBlack();m_mesh->setEdgeProperties(1, colorEdges);
 
 
图2.9 包围盒显示


模型网格显示实现代码：
//启用网格渲染模式
mesh->setWireMode(true);
//启用实体渲染模式
mesh->setWireMode(false);
 
图2.10 网格显示

虚拟手术仿真功能实现

 医生可在本虚拟手术系统上观察专家手术过程，也可使用与系统相关联的触觉设备重复实习。触觉设备是双向仪表/致动的人机界面，由操作人员使用以主动地与计算机模拟的虚拟环境交互。触觉设备可以是鼠标，方向盘，指腹，机器人手柄，抓手，操纵杆，手套，磁悬浮手腕，或甚至移动整个用户身体的运动平台的形式。它可用于模拟具有质量，阻尼，摩擦和刚度属性的环境，或者它可以模拟虚拟工具与其周围虚拟环境交互的机械行为。
基于CHAI3D提供的基类，实现了与大多数常见3D触觉设备通信的方法。本课题研究实现了力反馈接口与omega.x设备相连接，通过预设相关组织的材料属性或表面纹理，实现医学训练操作时的触感交互。

触觉设备

所开发的软件能实现与触觉设备omega.x的交互，该触觉设备携带手腕以及触笔，则其手柄的方向通过使用旋转矩阵来表示，共有六个自由度。
 
图2.11 触觉设备omega.x

软件与触觉设备omega.x交互的实现代码：
	// 创建触觉设备手柄
    cHapticDeviceHandler* handler = new cHapticDeviceHandler ();
	// 连接最近的触觉设备
    handler->getDevice (m_device, 0);
	// 创建工具
    m_tool = new cToolCursor (m_world);
    m_world->addChild (m_tool);
	// 连接触觉设备和虚拟工具
    m_tool->setHapticDevice(m_device);
	// 定义工具半径
    m_tool->setRadius(m_toolRadius);
	// 映射物理工作空间到虚拟工作空间.
    m_tool->setWorkspaceRadius(1.0);
	// 启动触觉工具
    m_tool->start();
	// 检索信息
    cHapticDeviceInfo hapticDeviceInfo = m_device->getSpecifications();
	// 读取比例系数
    double workspaceScaleFactor = m_tool->getWorkspaceScaleFactor();
	// 刚度属性
    m_maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;
	// 添加空物体
    m_mesh = new cMultiMesh();
    m_point = new cMultiPoint();
    m_world->addChild(m_mesh);
m_world->addChild(m_point);

虚拟手术器械

要实现虚拟手术器械工具的真实操作体感，需要将器械模型与虚拟世界的工具相结合。工具是3D对象，用于在虚拟世界中连接，建模和显示触觉设备。工具由称为触觉点（cHapticPoint）的一个或多个接触球以图形方式建模，其模拟触觉设备与环境之间的交互。计算触觉点和环境之间的相互作用力的一组力渲染算法（cAlgorithmFingerProxy和cAlgorithmPotentialField）与每个触觉点（cHapticPoint）相关联。遇到的对象类型决定了使用哪种强制渲染算法。一旦在工具的每个触觉点处计算了所有接触力，所产生的力被组合在一起并且被转换成发送到触觉设备的力，扭矩和夹持力。
开发的软件将工具实例化成医学手术器械，创建一个触觉线程，连续读取触觉设备的位置，计算工具和环境之间的交互力，并将计算出的力发送回触觉设备。如果需要对任何其他行为进行建模（例如，拾取或移动对象），则这些行为通常在相同的触觉循环中实现。
此外，不同的手术器械的材料不同，故需要针对不同的器械设置其材料属性以此来定义其刚度、摩擦系数等参数。本文所采取finger-proxy算法来实现力渲染。
finger-proxy算法其实现原理是使用所建立的虚拟对象（手术器械）代表触觉设备，当触觉设备的位置发生改变时，虚拟对象随之改变位置。虚拟对象的动作试图始终朝着目标前进时，若无阻碍其将直接朝向目标移动。当虚拟对象遇到障碍物时，不能进行直接运动，但是它可能够沿着手术对象模型表面移动一段小距离或者当操作者的操作力度一定时，手术器械可以穿进生物组织或其他器官（这与实际操作相同）。
本文被手术对象实时形变和力反馈算法是以弹簧-阻尼器为基础建立的。组织形变过程的总形变量是由虚拟弹簧形变叠加表现的,反馈力是受力点处虚拟弹簧形变产生的合力。交互系统在交互时要求能够提供视觉和触觉信息,为了保证真实性,二者保持同步,能给操作者带来沉浸感和临场感。本文利用OpenGL(Open Graphics Library)三维图形库在视觉上对组织模型进行绘制渲染,通过显示器提供视觉信息。总之，计算机实现时，可以在触觉设备（手术器械）和被手术对象之间建模虚拟弹簧来计算力。
弹簧的刚度在材料属性中定义，实现代码如下：
//创造一种触觉粘滑效果
mesh-> createEffectStickSlip（）;
//设置触觉属性
mesh-> m_material-> setStickSlipForceMax（5.0）;
mesh-> m_material-> setStickSlipStiffness（1000）;
//创建碰撞检测器
mesh-> createAABBCollisionDetector（toolRadius）;
//设置触觉属性
mesh-> m_material-> setStiffness（1000）;
mesh-> m_material-> setStaticFriction（0.3）;
mesh-> m_material-> setDynamicFriction（0.4）;

 
图2.12 虚拟手术器械

虚拟手术仿真

医生在完成被手术对象的模型导入后，可以选择所需的手术操作器械，如锥子、手术刀、手术钻等。
 
图2.13 虚拟手术仿真


交互设计
交互设计的思维方法建构于工业设计以用户为中心的方法，同时加以发展，更多地面向行为和过程，把产品看作一个事件，强调过程性思考的能力，流程图与状态转换图和故事板等成为重要设计表现手段，更重要的是掌握软件和硬件的原型实现的技巧方法和评估技术。
所设计的软件菜单有相关操作指示，方便用户使用。
 
交互界面 





参考文献
[1] 孙家广, 胡事民. 计算机图形学基础教程[M]. 清华大学出版社, 2009. 
[2] 杨华. 图像拼接技术在医疗图像处理中的应用[D]. 复旦大学, 2011.
[3] 苏文魁, 张毓笠, 李冬梅, et al. 手术导航系统及其应用概述[J]. 中国医疗器械杂志, 2010, 34(4):284-288. 
[4] 杨星, 马彪, 苏勤, et al. 螺旋CT三维重建的方法及技巧[J]. 中华放射学杂志, 1999, 33(7):492-493. 
[5] 林勇, 郭建忠, 魏海平, et al. OpenGL技术及地形三维可视化实现[J]. 海洋测绘, 2006, 26(3):68-70. 
[6] 何煦佳, 杨荣骞, 黄毅洲, et al. 基于OpenGL的医学图像实时交互处理技术[J]. 计算机应用与软件, 2013, 30(4):48-50. 
[7] 普拉塔. C++ Primer Plus (第五版) 中文版[M]. 人民邮电出版社, 2005.

