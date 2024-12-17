# generic-libuidbg
Qt插件，可查看Qt窗口对象树，以及对象属性

## 构建
1. cmke -S ${src} -B ${build} -G ${generator} -DCMAKE_PREFIX_PATH=${QT_PATH}
2. cmake --build ${build} --config release --parallel
3. cmake --build ${build} --config release --target deployqt

## 使用
1. 在模块中加载插件：QGenericPluginFactory::create("uidbg", "");
2. 组合键：Alt+Shift+F      对象树窗口，可查看对象组织结构，以及对象属性、方法
3. 组合键：Alt+Shift+D      日志窗口，重定向qDebug/qInfo...输出，并增加位置、时间信息