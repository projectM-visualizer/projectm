#ifndef PULSE_AUDIO_THREAD
#include <QObject>
#include <QTimer>
#include <QThread>
class projectM;

#include <pulse/simple.h>
#include <pulse/error.h>

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
		void init();
		int argc;
		char ** argv;
		QTimer * m_timer;
		pa_sample_spec ss ;
		pa_simple * s ;
		projectM * m_projectM;

};
#endif
