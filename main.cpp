#include "mainwindow.h"
#include <QApplication>

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    QByteArray localMsg = msg.toUtf8();
    std::string_view file{context.file ? context.file : ""};
    file = file.substr(file.find_last_not_of("\\/"));
    switch(type) {
    case QtDebugMsg:
        fprintf(stdout, "D: %s\n\t%s : %u\n", localMsg.constData(), file.data(), context.line /*, function*/);
        break;
    case QtInfoMsg:
        fprintf(stdout, "I: %s\n\t%s : %u\n", localMsg.constData(), file.data(), context.line /*, function*/);
        break;
    case QtWarningMsg:
        fprintf(stderr, "W:%s\n\t%s : %u\n", localMsg.constData(), file.data(), context.line /*, function*/);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "C:%s\n\t%s : %u\n", localMsg.constData(), file.data(), context.line /*, function*/);
        break;
    case QtFatalMsg:
        fprintf(stderr, "F:%s\n\t%s : %u\n", localMsg.constData(), file.data(), context.line /*, function*/);
        break;
    }
}

int main(int argc, char* argv[]) {
    qInstallMessageHandler(myMessageOutput);
    QApplication a{argc, argv};

    QApplication::setOrganizationName("Elemer");
    QApplication::setApplicationName("CalcFlow");

    QFont f;
    f.setPixelSize(16);
    QApplication::setFont(f);

    MainWindow w;
    w.show();
    return a.exec();
}
