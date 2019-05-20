#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QThread>
#include "spdlog/spdlog.h"
#include "spdlog//async.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog//fmt/fmt.h"

#include <QHostAddress>
#include "antcpserver.h"

//日志
using anlogger = std::shared_ptr<spdlog::logger>;
anlogger& getlogger() {
    static anlogger g_anlog;
    if (!g_anlog) {

        QString path = QCoreApplication::applicationDirPath();
        path+="\\logs\\anTcpServer";
        std::string logpath=path.toStdString();
        //std::string logpath(R"(D:\MyTest\2019_Qt\anSocket\logs\anTcpServer)");
        logpath += ".log";

        //是否已启动日志线程池?
        auto tp = spdlog::thread_pool();
        if (!tp) {
            spdlog::init_thread_pool(65536, 1);
        }
        g_anlog = spdlog::daily_logger_mt<spdlog::async_factory>("anTcpServer", logpath);

        g_anlog->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^---%L---%$] [%t] %v");
        g_anlog->set_level(spdlog::level::trace);
        g_anlog->flush_on(spdlog::level::info);

    }

    return g_anlog;
}
void anMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //return ;

#ifdef DEBUG
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
#endif

    QString data;
    QTextStream out(&data);

#ifdef DEBUG
    out << msg << " [" << file << ", " << function << ", " << context.line << "]";
#else
    out << msg ;
#endif

    switch (type) {
    case QtDebugMsg:
        getlogger()->debug(data.toStdString());
        //fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        getlogger()->info(data.toStdString());
        break;
    case QtWarningMsg:
        getlogger()->warn(data.toStdString());
        break;
    case QtCriticalMsg:
        getlogger()->critical(data.toStdString());
        break;
    case QtFatalMsg:
        getlogger()->error(data.toStdString());
        break;
    default:
        getlogger()->trace(data.toStdString());
        break;
    }

    getlogger()->flush();
}


int main(int argc, char *argv[])
{
    //logfilter
    qInstallMessageHandler(anMessageOutput);

    QCoreApplication a(argc, argv);

    //启动服务
    anTcpServer server;
    bool r = server.listen(QHostAddress::Any, 9555);
    if (r){
        qDebug().noquote() << "main() listen : 9555 succeed. tid=" << QThread::currentThreadId();
    }
    else{
        qDebug().noquote() << "main() listen : 9555 failed. tid=" << QThread::currentThreadId();
        return(-1);
    }

    return a.exec();
}
