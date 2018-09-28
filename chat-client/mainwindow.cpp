#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_user = new CUser;

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "WSAStartup failed" << endl;
    }

    m_user->m_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_user->m_sockfd == INVALID_SOCKET)
    {
        cout << "invalid socket" << endl;
    }

    m_user->m_servaddr.sin_family = AF_INET;
    m_user->m_servaddr.sin_port = htons(PORT);
    m_user->m_servaddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);

    ui->lineEditUsername->setText("admin");
    ui->lineEditPasswd->setText("admin");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonLogin_clicked()
{

    if (::connect(m_user->m_sockfd, (sockaddr *)&m_user->m_servaddr, sizeof(m_user->m_servaddr)) == SOCKET_ERROR)
    {
        cout << "connect error !" << endl;
        cout << WSAGetLastError() << endl;
        closesocket(m_user->m_sockfd);
    } else {
        cout << "connect success !" << endl;
    }

    memset(m_user->m_loginSendMsg.name, 0, 50);
    strcpy(m_user->m_loginSendMsg.name, "admin");
    memset(m_user->m_loginSendMsg.passwd, 0, 20);
    strcpy(m_user->m_loginSendMsg.passwd, "admin");

    int ret;
    m_user->m_loginSendMsg.loginHead.cmdId = LOGIN;
    m_user->m_loginSendMsg.loginHead.version = 1;
    m_user->m_loginSendMsg.loginHead.length  = sizeof(loginMessageSend);
    ret = send(m_user->m_sockfd, (const char *)&m_user->m_loginSendMsg, m_user->m_loginSendMsg.loginHead.length, 0);
    if (ret <= 0)
    {
        return;
    }
    ret = recv(m_user->m_sockfd, (char *)&m_user->m_loginRecvMsg, sizeof(loginMessageRecv), 0);
    if (ret <= 0)
    {
        return;
    }
    if(strcmp(m_user->m_loginRecvMsg.result, "Y") == 0)
    {
        cout << "��¼�ɹ���" << endl;
        cout << "your userId is: " << m_user->m_loginRecvMsg.loginHead.userId << endl;
        m_user->m_userId = m_user->m_loginRecvMsg.loginHead.userId;
        //fcntl(m_user->m_sockfd, F_SETFL, O_NONBLOCK);
    }else
    {
        cout << "input error" << endl;
        exit(0);
    }
}
