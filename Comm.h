#ifndef APICOM_H
#define APICOM_H

#include <QObject>
#include <QtNetwork/qtcpsocket.h>
#include <QtNetwork/QHostAddress>

#include "QMD5.h"
#include "QSentences.h"

namespace ROS
{

class Comm : public QObject
{
    Q_OBJECT
    QTcpSocket m_sock;
    QString m_addr;
    quint16 m_port;
    QString m_Username;
    QString m_Password;
	QByteArray incomingWord;
	QSentence incomingSentence;
	bool sentenceCompleted;

public:
	enum CommState
	{
		Unconnected,
		HostLookup,
		Connecting,
		Connected,
		Closing
	};

	enum LoginState
	{
		NoLoged,
		LoginRequested,
		UserPassSended,
		LogedIn
	};

private:
	LoginState m_loginState;
	int incomingWordSize;

	void doLogin();
	void tryLogin();
	void sendUser();
	void setLoginState(LoginState s);

protected:
	void writeLength(int wordLength);
    int readLength();
	void sendWord(const QString &strWord);
	int readWord();
	void readSentence();

private slots:
	void onError(QAbstractSocket::SocketError);
	void onReadyRead();
	void onSocketStateChanges(QAbstractSocket::SocketState s);

signals:
	void comError(const QString &error);
    void loginRequest(QString *user, QString *pass);
	void comReceive(ROS::QSentence &s);
	void comStateChanged(ROS::Comm::CommState s);
	void loginStateChanged(ROS::Comm::LoginState s);

public:
	Comm(QObject *papi = NULL);
	~Comm();

	/**
	 * @brief isConnected
	 * This function just tells if socket is connectet to ROS, but
	 * doesn't takes care if logon was done.
	 * @return true/false if socket is in "ConnectetState" or not.
	 */
	inline bool isConnected() const { return m_sock.state() == QAbstractSocket::ConnectedState;	}
	/**
	 * @brief isLoged
	 * This function checks if a succefull login was done.
	 * Internally, checks isConnected. Si, will return false if is not connected.
	 * @return true/false if we are correctly loged into ROS.
	 */
	inline bool isLoged() const { return isConnected() && (m_loginState == LogedIn); }
	inline bool isClosing() const { return m_sock.state() == QAbstractSocket::ClosingState; }
	inline bool isConnecting() const { return m_sock.state() == QAbstractSocket::ConnectingState; }

public slots:
	QString sendSentence(const ROS::QSentence &sent, bool addTag = true);
	bool connectTo(const QString &addr, quint16 port);
	void closeCom(bool force = false);
};
}
#endif // APICOM_H