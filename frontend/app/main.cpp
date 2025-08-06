int main(int argc, char**argv)
{
	QApplication app(argc, argv);
	xutility::InitializeApplication();
	xutility::ShowAppWindow();
    return app.exec();
}