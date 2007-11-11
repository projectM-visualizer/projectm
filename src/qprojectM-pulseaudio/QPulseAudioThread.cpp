#include "QPulseAudio.hpp"
#include <QtDebug>
#include "libprojectM/projectM.hpp"

#define BUFSIZE 1024
PulseAudioThread::PulseAudioThread(int _argc, char **_argv, projectM * _projectM, QObject * parent) : QThread(parent), argc(_argc), argv(_argv), s(0) , m_projectM(_projectM) {


	m_timer = new QTimer(this);
}

void PulseAudioThread::cleanup() {

	if ( s )
		pa_simple_free ( s );
	s = 0;
	
	qDebug() << "pulse audio quit";
	return ;
}

void PulseAudioThread::init() {

	ss.format = PA_SAMPLE_FLOAT32LE;
	ss.rate = 44100;
	ss.channels = 2;
	int ret = 1;
	int error;
	
	/* Create the recording stream */
	if ( ! ( s = pa_simple_new ( NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error ) ) )
	{
		fprintf ( stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror ( error ) );
		return;//RETURthis->exit(error);
	}


}

void PulseAudioThread::updatePCM() {
		//qDebug() << "pulse audio loop";
		float buf[BUFSIZE];
		ssize_t r;
		int error;
		qDebug() << "HERE";
		/* Record some data ... */
		if ( pa_simple_read ( s, buf, sizeof(buf), &error ) < 0 )
		{
			
			fprintf ( stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror ( error ) );
			return; //this->exit(error);
			
		}
		qDebug() << "HERE";
		m_projectM->pcm->addPCMfloat(buf, BUFSIZE);	
}


PulseAudioThread::~PulseAudioThread() 
{
	cleanup();
}

void PulseAudioThread::run()
{

	init();	
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePCM()));


	m_timer->start(0);

	//exec();
}
