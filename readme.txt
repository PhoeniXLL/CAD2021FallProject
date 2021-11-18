指定键盘或文件输入欧拉操作
新建元素的标号将以输出的形式告知用户
最终将生成obj文件并显示在屏幕上

操作调用规则
1.mev loopLabel point1Label point
	指定一个环上的点，新建一个点和一条边
2.mef loopLabel point1Label point2Label
	指定环上的两个点，将其相连产生一条新边和一个新面
3.mvfs point
	以一个点新建一个实体并产生一个面
4.kemr loopLabel edgeLabel
	指定环上的某条边将其删除生成一个内环
5.kfmrh face1Label face2Label
	指定两个紧贴的面，将第一个面删除，在第二个面上生成内环
6.sweep vector
	指定一个三维向量，将原来的面沿着该向量进行扫成操作
7.over
	结束欧拉操作，生成obj并显示在屏幕上
8.open filename
	令后续读入从文件中读取
9.debug
	可输出一些调试信息，较为直观地显示当前实体状态

由于交互式输入输出，提供几组输入数据便于检验
1.cube.txt
	立方体
2.cube_with_a_hole.txt
	立方体中间打通一个洞
3.cube_with_a_hole_sweep.txt
	同上，但使用扫成操作
4.letter_B.txt
	立体字母B(含有两个洞)，使用扫成操作
5.B_REP.txt
	短语B-REP，使用扫成操作

可以直接看对应的obj文件看到测试结果
