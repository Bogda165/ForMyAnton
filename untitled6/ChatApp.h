//
// Created by User on 25/11/2024.
//

#ifndef UNTITLED6_CHATAPP_H
#define UNTITLED6_CHATAPP_H
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QFile>
#include <QSocketNotifier>
#include <thread>
#include <chrono>
#include <QFileDialog>


class ChatApp : public QWidget {
    Q_OBJECT
private:
    QListWidget *chatList;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QPushButton *openFileButton;

    QFile pipeRead;
    QFile pipeWrite;
    QSocketNotifier *pipeNotifier;

    void addMessage(const QString &message, bool isSent) {
        // Create a list item
        auto *item = new QListWidgetItem(message, chatList);

        // Align based on sent/received
        if (isSent) {
            item->setTextAlignment(Qt::AlignRight);
        } else {
            item->setTextAlignment(Qt::AlignLeft);
        }

        // Optionally, customize font or color
        QFont font = item->font();
        font.setPointSize(12);
        item->setFont(font);

        if (isSent) {
            item->setForeground(Qt::blue); // Sent messages in blue
        } else {
            item->setForeground(Qt::green); // Received messages in green
        }

        chatList->scrollToBottom(); // Auto-scroll to the latest message
    }

    void writeToPipe(QByteArray buffer) {
        if(pipeWrite.isOpen()) {
            auto bytesWritten = pipeWrite.write(buffer);
            pipeWrite.flush();
            if (bytesWritten == -1) {
                qDebug() << "Error while writing to the pipe";
            }else {
                qDebug() << "Wrote to the fifo";
            }
        }else {
            qDebug() << "file is not open";
        }
    }
private slots:
    void openFileDialog(){
        QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "All Files (*)");
        if (!filePath.isEmpty()) {
            qDebug() << "Selected file:" << filePath;
            QByteArray buffer = filePath.toUtf8();
            buffer.prepend('1');
            writeToPipe(buffer);
        }
    }
    void sendMessage() {
        QString message = messageInput->text();
        if (!message.isEmpty()) {
            addMessage(message, true); // Add as a sent message
            //write to the pipe
            auto buffer = message.toUtf8();
            buffer.prepend('0');
            writeToPipe(buffer);
            messageInput->clear();
        }
    }

    // Simulate receiving a message
    void receiveMessage(const QString &message) {
        addMessage(message, false); // Add as a received message
    }

    void readFromPipe() {
        qDebug() << "fifo called";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        QByteArray buffer = pipeRead.readLine();
        qDebug() << "read " << buffer;

        addMessage(QString::fromUtf8(buffer), false);
    }

public:
    ~ChatApp() {
        pipeRead.close();
        pipeWrite.close();
    }
    ChatApp(const QString &filePathToRead, const QString &filePathToWrite, QWidget *parent = nullptr) : QWidget(parent), pipeRead(filePathToRead), pipeWrite(filePathToWrite) {
        qDebug() << "Construct chat app";

        // Main layout
        auto *mainLayout = new QVBoxLayout(this);

        // List to display messages
        chatList = new QListWidget(this);

        // Input area (line edit and button)
        auto *inputLayout = new QHBoxLayout();
        messageInput = new QLineEdit(this);
        sendButton = new QPushButton("send", this);
        openFileButton = new QPushButton("open File", this);

        inputLayout->addWidget(openFileButton);
        inputLayout->addWidget(messageInput);
        inputLayout->addWidget(sendButton);

        // Add widgets to the main layout
        mainLayout->addWidget(chatList);
        mainLayout->addLayout(inputLayout);

        // Connect button click to the send message function
        connect(sendButton, &QPushButton::clicked, this, &ChatApp::sendMessage);
        connect(messageInput, &QLineEdit::returnPressed, this, &ChatApp::sendMessage);
        connect(openFileButton, &QPushButton::clicked, this, &ChatApp::openFileDialog);

        qDebug() << "Attempting to open pipe for reading: " << filePathToRead;

        pipeWrite.open(QIODevice::WriteOnly);

        if(pipeWrite.isOpen()) {
            qDebug() << "Fifo was openend";
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        if(pipeRead.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
            qDebug() << "fifo opened";
            pipeNotifier = new QSocketNotifier(pipeRead.handle(), QSocketNotifier::Read, this);
            connect(pipeNotifier, &QSocketNotifier::activated, this, &ChatApp::readFromPipe);
        } else {
            qDebug() << "Failed to open fifo for reading";
            // Handle the error, e.g., show a message box or disable related functionality
        }
    }

};

#endif //UNTITLED6_CHATAPP_H
