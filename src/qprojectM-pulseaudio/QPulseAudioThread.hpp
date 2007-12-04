#ifndef PULSE_AUDIO_THREAD
#include <QObject>
#include <QTimer>
#include <QThread>
#include <QString>
#include <QVector>

class projectM;


class QPulseAudioThread : public QThread
{	
	Q_OBJECT
	public:
		QPulseAudioThread () {}
		QPulseAudioThread(int _argc, char **_argv, projectM * projectM, QObject *parent);
		virtual ~QPulseAudioThread() ;
		void run();
		void cleanup();
		inline QVector<QString> & getSourceList() {
			return sourceList;
		}
	public slots:
		void connectSource(int index);
	private:
		QVector<QString> sourceList;
		int sourceIndex;
		int argc;
		char ** argv;
		projectM * m_projectM;

};
#endif
