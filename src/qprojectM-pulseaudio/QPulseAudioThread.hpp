#ifndef PULSE_AUDIO_THREAD
#include <QObject>
#include <QTimer>
#include <QThread>
class projectM;


class QPulseAudioThread : public QThread
{	
	Q_OBJECT
	public:
		QPulseAudioThread () {}
		QPulseAudioThread(int _argc, char **_argv, projectM * projectM, QObject *parent);
		~QPulseAudioThread() ;
		void run();
		void cleanup();

	private:
		int argc;
	char ** argv;
		projectM * m_projectM;

};
#endif
