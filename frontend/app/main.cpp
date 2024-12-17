
int main(int argc, char**argv)
{
    QApplication app(argc, argv);
    QGenericPluginFactory::create("uidbg", "");
    QResource::registerResource(app.applicationDirPath() + "/resource/app.rcc");
    QFile file(":/app.qss");
    if (file.open(QIODevice::ReadOnly)) {
        const auto content = file.readAll();
        app.setStyleSheet(content);
        qLogInfo << content;
        file.close();
    }
    
    QScopedPointer<QWidget> widget(Factory<QWidget>::produce("VXWidget"));
    if (widget) widget->show();

    return app.exec();
}
